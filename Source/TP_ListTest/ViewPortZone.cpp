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

    //対応するオブジェクトを作成
    UEntryObject* entryItem = NewObject<UEntryObject>();
    entryItem->m_owningActor = actor;
    entryItem->m_owningViewPortZoneInstance = this;
    entryItem->m_headerText = FText::FromString(actor->GetName());
    entryItem->m_bodyText = FText::FromString(actor->GetName());
    entryItem->m_isExpand = false;
    entryItem->m_isSelect = true;

    m_owningListViewInstance->m_listView->AddItem(entryItem);

    m_entryObjects.Add(entryItem);

    //配置したアクタを選択状態にする
    SelectActorWithCtrlKey(actor, isMultiSelectState);
}

void UViewPortZone::RemoveActor(AActor* actor)
{
    UEntryObject* removeItem = FindEntryObjectByActor(actor);
    m_owningListViewInstance->m_listView->RemoveItem(removeItem);
    m_entryObjects.Remove(removeItem);
    m_actors.Remove(actor);
    actor->Destroy();

    //再描画(MEMO:再描画しないとリストの見た目が更新されない)
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
        FVector spawnLocation = selectedActor->GetActorLocation() + FVector(20.0f, 20.0f, 0.0f);//少しずらす
        FRotator spawnRotation = selectedActor->GetActorRotation();

        FActorSpawnParameters spawnParams;

        AActor* copyActor = GetWorld()->SpawnActor<AActor>(actorClass, spawnLocation, spawnRotation, spawnParams);
        reSelectActors.Add(copyActor);
    }

    //コピーされたアクタを全て選択状態とする
    for (AActor* reSelectActor : reSelectActors)
    {
        if (isFirst)
        {
            //最初だけ複数選択フラグをfalseにすることでコピー元の選択状態は解除する
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
    if (m_CurrentSelectedActors.Num() >= 2)//MEMO:1つ目はすでに選択されていたリスト要素、2つ目は今回Shiftキー押した状態でクリックされたリスト要素
    {
        int closeMinIndex = INT_MAX;
        int closeMaxIndex = INT_MIN;
        int targetIndex = m_owningListViewInstance->m_listView->GetIndexForItem(selectedEntryObject);

        AActor* actor = m_CurrentSelectedActors[m_CurrentSelectedActors.Num() - 2];
        UEntryObject* entryObject = FindEntryObjectByActor(actor);
        int checkIndex = m_owningListViewInstance->m_listView->GetIndexForItem(entryObject);
        //近い値(小さい方)を求める
        if (checkIndex < targetIndex && (closeMinIndex == INT_MAX || checkIndex > closeMinIndex))
        {
            closeMinIndex = checkIndex;
        }
        //近い値(大きい方)を求める
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

        //対象のリストも選択状態にする
        ForceSelectActor(selectedEntryObject->m_owningActor, true);
    }
    else
    {
        //何も選択がない状態でこの関数が呼ばれた場合は対象のリスト要素まで上から全て選択状態にする
        int targetIndex = 0;
        int closeMaxIndex = m_owningListViewInstance->m_listView->GetIndexForItem(selectedEntryObject);
        for (int i = targetIndex; closeMaxIndex > i; i++)
        {
            ForceSelectActor(m_entryObjects[i]->m_owningActor, true);
        }

        //対象のリストも選択状態にする
        ForceSelectActor(selectedEntryObject->m_owningActor, true);
    }
}

void UViewPortZone::SelectAllActors(bool isSelect)
{
    //ひとつも選択されていないなら何もしない
    if (m_CurrentSelectedActors.Num() <= 0)
    {
        return;
    }

    //全てのアクタを選択、または非選択状態にする
    for (AActor* actor : m_actors)
    {
        ForceSelectActor(actor, isSelect);
    }
}

bool UViewPortZone::SetActorOutLine(AActor* actor, bool visible)
{
    bool bHasMeshComponent = false;
    FString ActorName = actor->GetName();

    // アクタのコンポーネントをチェックする
    TArray<UActorComponent*> Components;
    actor->GetComponents(Components); // TSetからTArrayに変換

    for (UActorComponent* Component : Components)
    {
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
        {
            if (StaticMeshComponent->GetStaticMesh() != nullptr)
            {
                bHasMeshComponent = true;

                //3D空間上のアクタにアウトラインを設定
                StaticMeshComponent->SetRenderCustomDepth(visible); // Custom Depth Pass を有効にする
                StaticMeshComponent->SetCustomDepthStencilValue(1); // 任意の値に設定

                break;
            }
        }
        else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
        {
            if (SkeletalMeshComponent->GetSkinnedAsset() != nullptr)
            {
                bHasMeshComponent = true;

                //3D空間上のアクタにアウトラインを設定
                SkeletalMeshComponent->SetRenderCustomDepth(visible); // Custom Depth Pass を有効にする
                SkeletalMeshComponent->SetCustomDepthStencilValue(1); // 任意の値に設定
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
       //falseとするとウィンドウ側の操作を受け付けるようになる
        return FEventReply(false);
    }

    UWorld* World = GetWorld();
    if (World)
    {
        // スクリーン座標から世界座標への変換
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
        FVector2D ScreenLocation;
        PlayerController->GetMousePosition(ScreenLocation.X, ScreenLocation.Y);
        FVector WorldDirection, WorldLocation;
        if (PlayerController->DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, WorldLocation, WorldDirection))
        {
            // レイキャストを使用して3D空間内のActorを取得
            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.bReturnPhysicalMaterial = false;
            
            // 現在のビューターゲットアクターを無視する
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
    //追加フラグ
    bool isAdd = true;

    //Ctrlキーが押されていない場合は選択されていたものを全てクリア
    if (!isCtrlKeyDown)
    {
        for (AActor* selectedActor : m_CurrentSelectedActors)
        {
            if (selectedActor)
            {
                //各アクタのアウトラインを外す
                SetActorOutLine(selectedActor, false);
                //アウトライナから選択を外す
                SelectListViewActor(selectedActor, false);
            }
        }
        //配列をクリア
        m_CurrentSelectedActors.Empty();
    }
    else
    {
        //既に選択されているアクタであった場合は非選択状態とする
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

    //追加
    if (isAdd)
    {
        m_CurrentSelectedActors.Add(actor);
        SetActorOutLine(actor, true);
        SelectListViewActor(actor, true);
    }

    //リストを動かし対象アクタ名が見えるようにする
    UEntryObject* findEntryObject = FindEntryObjectByActor(actor);
    if (findEntryObject)
    {
        m_owningListViewInstance->m_listView->RequestScrollItemIntoView(findEntryObject);
    }
    
    //再描画(MEMO:再描画しないとリストの見た目が更新されず選択色にならなかったりする)
    m_owningListViewInstance->m_listView->RegenerateAllEntries();
    
}

void UViewPortZone::ForceSelectActor(AActor* actor, bool isSelect)
{
    m_CurrentSelectedActors.AddUnique(actor);
    SetActorOutLine(actor, isSelect);
    SelectListViewActor(actor, isSelect);

    //再描画(MEMO:再描画しないとリストの見た目が更新されず選択色にならなかったりする)
    m_owningListViewInstance->m_listView->RegenerateAllEntries();
}

