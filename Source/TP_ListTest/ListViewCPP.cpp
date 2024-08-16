// Fill out your copyright notice in the Description page of Project Settings.


#include "ListViewCPP.h"
#include "Components/ListView.h"
#include "UObject/ConstructorHelpers.h"
#include "ListViewEntry.h"
#include <Kismet/KismetSystemLibrary.h>
#include "EntryObject.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include <Blueprint\WidgetBlueprintLibrary.h>

void UListViewCPP::NativeConstruct()
{
    Super::NativeConstruct();

    //ViewPortzone���擾
    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UViewPortZone::StaticClass(), true);
    m_viewPortZone = Cast<UViewPortZone>(Widgets[0]);

    //�A�N�^�z����N���A
    m_viewPortZone->m_actors.Empty();
    FilterActorsWithMeshes(m_viewPortZone->m_actors);

    //ViewList�X�V��ViewPortZone������{���Ă��炤����
    m_viewPortZone->m_owningListViewInstance = this;

    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), m_actors);

    if (m_listView)
    {
        TArray<UEntryObject*> entryItems;
        m_listView->ClearListItems();

        // ���X�g�ɕ\������A�C�e�����쐬
        for (AActor* Actor : m_viewPortZone->m_actors)
        {
            UEntryObject* entryItem = NewObject<UEntryObject>();

            entryItem->m_owningActor = Actor;
            entryItem->m_owningViewPortZoneInstance = m_viewPortZone;
            entryItem->m_headerText = FText::FromString(Actor->GetName());
            entryItem->m_bodyText = FText::FromString(Actor->GetName());
            entryItem->m_isExpand = false;
            entryItem->m_isSelect = false;

            entryItems.Add(entryItem);
        }

        // �����Őݒ肳�ꂽ�I�u�W�F�N�g��Entry�v�f��NativeOnListItemObjectSet�ɓn�����
        m_listView->SetListItems(entryItems);

        //�Ǘ������邽�ߔz���n��
        m_viewPortZone->m_entryObjects = entryItems;

        //C++ListView�Q�l:https://forums.unrealengine.com/t/list-view-please-help-me-implement-it-in-c/155947/6
    
        
    }
}

void UListViewCPP::FilterActorsWithMeshes(TArray<AActor*>& OutActors)
{
    // ���ׂẴA�N�^���擾����
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    // ���b�V�������A�N�^�݂̂��t�B���^�����O����
    OutActors.Empty();
    for (AActor* Actor : AllActors)
    {
        bool bHasMeshComponent = false;

        // �A�N�^�̃R���|�[�l���g���`�F�b�N����
        TArray<UActorComponent*> Components;
        Actor->GetComponents(Components); // TSet����TArray�ɕϊ�
        for (UActorComponent* Component : Components)
        {
            if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component))
            {
                if (StaticMeshComponent->GetStaticMesh() != nullptr)
                {
                    bHasMeshComponent = true;
                    break;
                }
            }
            else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
            {
                if (SkeletalMeshComponent->GetSkinnedAsset() != nullptr)
                {
                    bHasMeshComponent = true;
                    break;
                }
            }
        }

        if (bHasMeshComponent)
        {
            OutActors.Add(Actor);
        }
    }
}

FReply  UListViewCPP::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    UKismetSystemLibrary::PrintString(this, "C++ Hello World!", true, true, FColor::Cyan, 2.f, TEXT("None"));

    return FReply::Handled();
}



