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

    //ViewPortzoneを取得
    TArray<UUserWidget*> Widgets;
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), Widgets, UViewPortZone::StaticClass(), true);
    m_viewPortZone = Cast<UViewPortZone>(Widgets[0]);

    //アクタ配列をクリア
    m_viewPortZone->m_actors.Empty();
    FilterActorsWithMeshes(m_viewPortZone->m_actors);

    //ViewList更新をViewPortZoneから実施してもらうため
    m_viewPortZone->m_owningListViewInstance = this;

    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), m_actors);

    if (m_listView)
    {
        TArray<UEntryObject*> entryItems;
        m_listView->ClearListItems();

        // リストに表示するアイテムを作成
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

        // ここで設定されたオブジェクトはEntry要素のNativeOnListItemObjectSetに渡される
        m_listView->SetListItems(entryItems);

        //管理させるため配列を渡す
        m_viewPortZone->m_entryObjects = entryItems;

        //C++ListView参考:https://forums.unrealengine.com/t/list-view-please-help-me-implement-it-in-c/155947/6
    
        
    }
}

void UListViewCPP::FilterActorsWithMeshes(TArray<AActor*>& OutActors)
{
    // すべてのアクタを取得する
    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

    // メッシュを持つアクタのみをフィルタリングする
    OutActors.Empty();
    for (AActor* Actor : AllActors)
    {
        bool bHasMeshComponent = false;

        // アクタのコンポーネントをチェックする
        TArray<UActorComponent*> Components;
        Actor->GetComponents(Components); // TSetからTArrayに変換
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



