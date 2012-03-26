//
//  AvidaEDOrganismValueDisplayCollectionView.m
//  viewer-macos
//
//  Created by David Michael Bryson on 3/22/12.
//  Copyright (c) 2012 Michigan State University. All rights reserved.
//

#import "AvidaEDOrganismValueDisplayCollectionView.h"

#import "AvidaEDOrganismValueDisplayView.h"


@implementation AvidaEDOrganismValueDisplayCollectionView

- (NSCollectionViewItem *)newItemForRepresentedObject:(id)object {
  
  // Get a copy of the item prototype, set represented object
  NSCollectionViewItem* newItem = [[self itemPrototype] copy];
  [newItem setRepresentedObject:object];
  
  // Get the new item's view so you can mess with it
  AvidaEDOrganismValueDisplayView* itemView = (AvidaEDOrganismValueDisplayView*)[newItem view];
  
  [[itemView valueDisplay] bind:@"displayValue" toObject:newItem withKeyPath:@"representedObject.value" options:nil];
  
  return newItem;
}

@end
