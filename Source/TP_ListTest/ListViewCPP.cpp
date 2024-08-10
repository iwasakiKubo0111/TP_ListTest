// Fill out your copyright notice in the Description page of Project Settings.


#include "ListViewCPP.h"
#include "Components/ListView.h"
#include "UObject/ConstructorHelpers.h"
#include "ListViewEntry.h"
#include <Kismet/KismetSystemLibrary.h>
#include "EntryObject.h"

void UListViewCPP::NativeConstruct()
{
    Super::NativeConstruct();

    if (m_listView)
    {
        TArray<UEntryObject*> entryItems;
        m_listView->ClearListItems();

        // リストに表示するアイテムを作成
        for (int i = 0; i < 60; ++i)
        {
            UEntryObject* entryItem = NewObject<UEntryObject>();

            entryItem->m_headerText = FText::AsNumber(i);
            entryItem->m_bodyText = FText::AsNumber(i + 2);
            entryItem->m_IsExpand = false;

            entryItems.Add(entryItem);
        }

        // ここで設定されたオブジェクトはEntry要素のNativeOnListItemObjectSetに渡される
        m_listView->SetListItems(entryItems);

        //C++ListView参考:https://forums.unrealengine.com/t/list-view-please-help-me-implement-it-in-c/155947/6
    }
}
