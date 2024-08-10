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

        // ���X�g�ɕ\������A�C�e�����쐬
        for (int i = 0; i < 60; ++i)
        {
            UEntryObject* entryItem = NewObject<UEntryObject>();

            entryItem->m_headerText = FText::AsNumber(i);
            entryItem->m_bodyText = FText::AsNumber(i + 2);
            entryItem->m_IsExpand = false;

            entryItems.Add(entryItem);
        }

        // �����Őݒ肳�ꂽ�I�u�W�F�N�g��Entry�v�f��NativeOnListItemObjectSet�ɓn�����
        m_listView->SetListItems(entryItems);

        //C++ListView�Q�l:https://forums.unrealengine.com/t/list-view-please-help-me-implement-it-in-c/155947/6
    }
}
