// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewPortZone.h"
#include <Kismet\KismetSystemLibrary.h>
#include <Kismet\GameplayStatics.h>
#include "ListViewCPP.h"

void UViewPortZone::NativeConstruct()
{
    Super::NativeConstruct();
	m_viewPortImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnImageClicked"));
}

void UViewPortZone::AddActor(AActor* actor ,bool isMultiSelectState)
{
    m_actors.Add(actor);

    //�Ή�����I�u�W�F�N�g���쐬
    UEntryObject* entryItem = NewObject<UEntryObject>();
    entryItem->m_owningActor = actor;
    entryItem->m_owningViewPortZoneInstance = this;
    entryItem->m_headerText = FText::FromString(actor->GetName());
    entryItem->m_bodyText = FText::FromString(actor->GetName());
    entryItem->m_isExpand = false;
    entryItem->m_isSelect = true;

    m_owningListViewInstance->m_listView->AddItem(entryItem);

    m_entryObjects.Add(entryItem);

    //�z�u�����A�N�^��I����Ԃɂ���
    SelectActorWithCtrlKey(actor, isMultiSelectState);
}

void UViewPortZone::RemoveActor(AActor* actor)
{
    UEntryObject* removeItem = FindEntryObjectByActor(actor);
    m_owningListViewInstance->m_listView->RemoveItem(removeItem);
    m_entryObjects.Remove(removeItem);
    m_actors.Remove(actor);
    actor->Destroy();

    //�ĕ`��(MEMO:�ĕ`�悵�Ȃ��ƃ��X�g�̌����ڂ��X�V����Ȃ�)
    m_owningListViewInstance->m_listView->RegenerateAllEntries();
}

void UViewPortZone::RemoveCurrentSelectedActors()
{
    if (m_CurrentSelectedActors.Num() != 0)
    {
        for (AActor* selectedActor : m_CurrentSelectedActors)
        {
            RemoveActor(selectedActor);
        }
    }
}

void UViewPortZone::CopyCurrentSelectedActors()
{
    TArray<AActor*> reSelectActors;
    bool isFirst = true;

    for (AActor* selectedActor : m_CurrentSelectedActors)
    {
        TSubclassOf<AActor> actorClass = selectedActor->GetClass();
        FVector spawnLocation = selectedActor->GetActorLocation() + FVector(20.0f, 20.0f, 0.0f);//�������炷
        FRotator spawnRotation = selectedActor->GetActorRotation();

        FActorSpawnParameters spawnParams;

        AActor* copyActor = GetWorld()->SpawnActor<AActor>(actorClass, spawnLocation, spawnRotation, spawnParams);
        reSelectActors.Add(copyActor);
    }

    //�R�s�[���ꂽ�A�N�^��S�đI����ԂƂ���
    for (AActor* reSelectActor : reSelectActors)
    {
        if (isFirst)
        {
            //�ŏ����������I���t���O��false�ɂ��邱�ƂŃR�s�[���̑I����Ԃ͉�������
            AddActor(reSelectActor, false);
            isFirst = false;
        }
        else
        {
            AddActor(reSelectActor, true);
        }
    }

    m_owningListViewInstance->m_listView->ScrollToBottom();

}

void UViewPortZone::SelectListViewActorWithShiftKey(UEntryObject* selectedEntryObject)
{
    if (m_CurrentSelectedActors.Num() >= 2)//MEMO:1�ڂ͂��łɑI������Ă������X�g�v�f�A2�ڂ͍���Shift�L�[��������ԂŃN���b�N���ꂽ���X�g�v�f
    {
        int closeMinIndex = INT_MAX;
        int closeMaxIndex = INT_MIN;
        int targetIndex = m_owningListViewInstance->m_listView->GetIndexForItem(selectedEntryObject);

        AActor* actor = m_CurrentSelectedActors[m_CurrentSelectedActors.Num() - 2];
        UEntryObject* entryObject = FindEntryObjectByActor(actor);
        int checkIndex = m_owningListViewInstance->m_listView->GetIndexForItem(entryObject);
        //�߂��l(��������)�����߂�
        if (checkIndex < targetIndex && (closeMinIndex == INT_MAX || checkIndex > closeMinIndex))
        {
            closeMinIndex = checkIndex;
        }
        //�߂��l(�傫����)�����߂�
        if (checkIndex > targetIndex && (closeMaxIndex == INT_MIN || checkIndex < closeMaxIndex))
        {
            closeMaxIndex = checkIndex;
        }

        for (int i = targetIndex; closeMinIndex < i ; i--)
        {
            ForceSelectActor(m_entryObjects[i]->m_owningActor,true);
        }

        for (int i = targetIndex; closeMaxIndex > i ; i++)
        {
            ForceSelectActor(m_entryObjects[i]->m_owningActor, true);
        }

        //�Ώۂ̃��X�g���I����Ԃɂ���
        ForceSelectActor(selectedEntryObject->m_owningActor, true);
    }
    else
    {
        //�����I�����Ȃ���Ԃł��̊֐����Ă΂ꂽ�ꍇ�͑Ώۂ̃��X�g�v�f�܂ŏォ��S�đI����Ԃɂ���
        int targetIndex = 0;
        int closeMaxIndex = m_owningListViewInstance->m_listView->GetIndexForItem(selectedEntryObject);
        for (int i = targetIndex; closeMaxIndex > i; i++)
        {
            ForceSelectActor(m_entryObjects[i]->m_owningActor, true);
        }

        //�Ώۂ̃��X�g���I����Ԃɂ���
        ForceSelectActor(selectedEntryObject->m_owningActor, true);
    }
}

void UViewPortZone::SelectAllActors(bool isSelect)
{
    //�ЂƂ��I������Ă��Ȃ��Ȃ牽�����Ȃ�
    if (m_CurrentSelectedActors.Num() <= 0)
    {
        return;
    }

    //�S�ẴA�N�^��I���A�܂��͔�I����Ԃɂ���
    for (AActor* actor : m_actors)
    {
        ForceSelectActor(actor, isSelect);
    }
}

bool UViewPortZone::SetActorOutLine(AActor* actor, bool visible)
{
    bool bHasMeshComponent = false;
    FString ActorName = actor->GetName();

    // �A�N�^�̃R���|�[�l���g���`�F�b�N����
    TArray<UActorComponent*> Components;
    actor->GetComponents(Components); // TSet����TArray�ɕϊ�

    for (UActorComponent* Component : Components)
    {
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
        {
            if (StaticMeshComponent->GetStaticMesh() != nullptr)
            {
                bHasMeshComponent = true;

                //3D��ԏ�̃A�N�^�ɃA�E�g���C����ݒ�
                StaticMeshComponent->SetRenderCustomDepth(visible); // Custom Depth Pass ��L���ɂ���
                StaticMeshComponent->SetCustomDepthStencilValue(1); // �C�ӂ̒l�ɐݒ�

                break;
            }
        }
        else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
        {
            if (SkeletalMeshComponent->GetSkinnedAsset() != nullptr)
            {
                bHasMeshComponent = true;

                //3D��ԏ�̃A�N�^�ɃA�E�g���C����ݒ�
                SkeletalMeshComponent->SetRenderCustomDepth(visible); // Custom Depth Pass ��L���ɂ���
                SkeletalMeshComponent->SetCustomDepthStencilValue(1); // �C�ӂ̒l�ɐݒ�
                break;
            }
        }
    }

    if (bHasMeshComponent)
    {
        return true;
    }
    else
    {
        return false;
    }
}


FEventReply UViewPortZone::OnImageClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
    {
       //false�Ƃ���ƃE�B���h�E���̑�����󂯕t����悤�ɂȂ�
        return FEventReply(false);
    }

    UWorld* World = GetWorld();
    if (World)
    {
        // �X�N���[�����W���琢�E���W�ւ̕ϊ�
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
        FVector2D ScreenLocation;
        PlayerController->GetMousePosition(ScreenLocation.X, ScreenLocation.Y);
        FVector WorldDirection, WorldLocation;
        if (PlayerController->DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, WorldLocation, WorldDirection))
        {
            // ���C�L���X�g���g�p����3D��ԓ���Actor���擾
            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.bReturnPhysicalMaterial = false;
            
            // ���݂̃r���[�^�[�Q�b�g�A�N�^�[�𖳎�����
            AActor* ViewTarget = PlayerController->GetViewTarget();
            if (ViewTarget)
            {
                QueryParams.AddIgnoredActor(ViewTarget);
            }

            if (World->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + WorldDirection * 10000.0f, ECC_Visibility, QueryParams))
            {
                AActor* HitActor = HitResult.GetActor();
                if (HitActor)
                {
                    SelectActorWithCtrlKey(HitActor, MouseEvent.IsControlDown());
                }
            }
        }
    }

    return FEventReply(false);
}

void UViewPortZone::SelectListViewActor(AActor* actor, bool isSelect)
{
    UEntryObject* findEntryObject = FindEntryObjectByActor(actor);
    if (findEntryObject)
    {
        findEntryObject->m_isSelect = isSelect;
    }
}

UEntryObject* UViewPortZone::FindEntryObjectByActor(AActor* actor)
{
    for (UEntryObject* entryObject : m_entryObjects)
    {
        if (entryObject->m_owningActor == actor)
        {
            return entryObject;
        }
    }

    return nullptr;
}

void UViewPortZone::SelectActorWithCtrlKey(AActor* actor, bool isCtrlKeyDown)
{
    //�ǉ��t���O
    bool isAdd = true;

    //Ctrl�L�[��������Ă��Ȃ��ꍇ�͑I������Ă������̂�S�ăN���A
    if (!isCtrlKeyDown)
    {
        for (AActor* selectedActor : m_CurrentSelectedActors)
        {
            if (selectedActor)
            {
                //�e�A�N�^�̃A�E�g���C�����O��
                SetActorOutLine(selectedActor, false);
                //�A�E�g���C�i����I�����O��
                SelectListViewActor(selectedActor, false);
            }
        }
        //�z����N���A
        m_CurrentSelectedActors.Empty();
    }
    else
    {
        //���ɑI������Ă���A�N�^�ł������ꍇ�͔�I����ԂƂ���
        for (AActor* selectedActor : m_CurrentSelectedActors)
        {
            if (selectedActor == actor)
            {
                m_CurrentSelectedActors.Remove(selectedActor);
                SetActorOutLine(actor, false);
                SelectListViewActor(selectedActor, false);
                isAdd = false;
            }
        }
    }

    //�ǉ�
    if (isAdd)
    {
        m_CurrentSelectedActors.Add(actor);
        SetActorOutLine(actor, true);
        SelectListViewActor(actor, true);
    }

    //���X�g�𓮂����ΏۃA�N�^����������悤�ɂ���
    UEntryObject* findEntryObject = FindEntryObjectByActor(actor);
    if (findEntryObject)
    {
        m_owningListViewInstance->m_listView->RequestScrollItemIntoView(findEntryObject);
    }
    
    //�ĕ`��(MEMO:�ĕ`�悵�Ȃ��ƃ��X�g�̌����ڂ��X�V���ꂸ�I��F�ɂȂ�Ȃ������肷��)
    m_owningListViewInstance->m_listView->RegenerateAllEntries();
    
}

void UViewPortZone::ForceSelectActor(AActor* actor, bool isSelect)
{
    m_CurrentSelectedActors.AddUnique(actor);
    SetActorOutLine(actor, isSelect);
    SelectListViewActor(actor, isSelect);

    //�ĕ`��(MEMO:�ĕ`�悵�Ȃ��ƃ��X�g�̌����ڂ��X�V���ꂸ�I��F�ɂȂ�Ȃ������肷��)
    m_owningListViewInstance->m_listView->RegenerateAllEntries();
}

