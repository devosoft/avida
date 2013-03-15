//
//  ACProject.mm
//  avida/apps/viewer-macos/frameworks/AvidaCore-ObjC
//
//  Created by David M. Bryson on 2/25/13.
//  Copyright 2013 Michigan State University. All rights reserved.
//  http://avida.devosoft.org/viewer-macos
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
//  following conditions are met:
//
//  1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the
//      following disclaimer.
//  2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//      following disclaimer in the documentation and/or other materials provided with the distribution.
//  3.  Neither the name of Michigan State University, nor the names of contributors may be used to endorse or promote
//      products derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY MICHIGAN STATE UNIVERSITY AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL MICHIGAN STATE UNIVERSITY OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Authors: David M. Bryson <david@programerror.com>
//

#import "ACProject.h"
#import "ACProject_Private.h"


NSString* const AvidaPasteboardTypeProjectItem = @"org.devosoft.avida.projectitem";


// ACProjectItem Interface
// --------------------------------------------------------------------------------------------------------------

@interface ACProjectItem : NSObject {
  NSString* title;
  Avida::Viewer::FreezerID freezer_id;
  
  NSInteger badgeValue;
  NSImage* icon;
  
  NSArray* children;
}

- (ACProjectItem*) init;

+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle;
+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle icon:(NSImage*)itemIcon;
+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle;
+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle icon:(NSImage*)itemIcon;


@property (nonatomic, copy) NSString* title;
@property (nonatomic, assign) Avida::Viewer::FreezerID freezer_id;
@property NSInteger badgeValue;
@property (nonatomic, retain) NSImage* icon;
@property (nonatomic, copy) NSArray* children;

- (BOOL) hasBadge;
- (BOOL) hasChildren;
- (BOOL) hasIcon;

// --------------------------------------------------------------------------------------------------------------
@end;


// ACProjectItem Implementation
// --------------------------------------------------------------------------------------------------------------

@implementation ACProjectItem

@synthesize title;
@synthesize freezer_id;
@synthesize badgeValue;
@synthesize icon;
@synthesize children;


// ACProjectItem Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - ACProjectItem Initialization

- (ACProjectItem*) init
{
  self = [super init];
  
  if (self) {
    badgeValue = -1;
  }
  
  return self;
}


+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle
{
  return [ACProjectItem itemWithTitle:itemTitle icon:nil];
}


+ (ACProjectItem*) itemWithTitle:(NSString*)itemTitle icon:(NSImage*)itemIcon
{
  ACProjectItem* item = [[ACProjectItem alloc] init];
  
  if (item) {
    item.title = itemTitle;
    item.icon = itemIcon;
  }
  
  return item;
}


+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle
{
  return [ACProjectItem itemWithFreezerID:fid title:itemTitle icon:nil];
}


+ (ACProjectItem*) itemWithFreezerID:(Avida::Viewer::FreezerID)fid title:(NSString*)itemTitle icon:(NSImage*)itemIcon
{
  ACProjectItem* item = [[ACProjectItem alloc] init];
  
  if (item) {
    item.title = itemTitle;
    item.freezer_id = fid;
    item.icon = itemIcon;
  }
  
  return item;
}



// ACProjectItem Accessors
// --------------------------------------------------------------------------------------------------------------
#pragma mark - ACProjectItem Accessors

- (BOOL) hasBadge
{
  return (badgeValue != -1);
}


- (BOOL) hasChildren
{
  return [children count] > 0;
}


- (BOOL) hasIcon
{
  return (icon != nil);
}


// --------------------------------------------------------------------------------------------------------------
@end


// ACProject Implementation
// --------------------------------------------------------------------------------------------------------------

@implementation ACProject


// Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Initialization

+ (ACProject*) projectWithFreezer:(Avida::Viewer::FreezerPtr)new_freezer
{
  ACProject* project = [[ACProject alloc] init];
  
  if (project) {
    project->freezer = new_freezer;
    
    project->configItem = [ACProjectItem itemWithTitle:@"Configurations"];
    project->worldItem = [ACProjectItem itemWithTitle:@"Saved Worlds"];
    project->genomeItem = [ACProjectItem itemWithTitle:@"Genomes"];
    
    const Avida::Viewer::FreezerObjectType object_types[] = { Avida::Viewer::CONFIG, Avida::Viewer::WORLD, Avida::Viewer::GENOME };
    NSArray* itemGroups = @[project->configItem, project->worldItem, project->genomeItem];
    
    for (NSUInteger idx = 0; idx < [itemGroups count]; idx++) {
      Avida::Viewer::FreezerObjectType entry_type = object_types[idx];
      NSMutableArray* items = [NSMutableArray arrayWithCapacity:new_freezer->NumEntriesOfType(entry_type)];
      for (Avida::Viewer::Freezer::Iterator it = new_freezer->EntriesOfType(entry_type); it.Next();) {
        Avida::Viewer::FreezerID fid = *it.Get();
        NSString* itemName = [NSString stringWithAptoString:new_freezer->NameOf(fid)];
        ACProjectItem* item = [ACProjectItem itemWithFreezerID:fid title:itemName];
        [items addObject:item];
      }
      [[itemGroups objectAtIndex:idx] setChildren:items];
    }
    
    project->sourceListItems = itemGroups;
  }
  
  return project;
}



// AptoSourceListDataSource
// --------------------------------------------------------------------------------------------------------------
#pragma mark - AptoSourceListDataSource

- (NSUInteger) sourceList:(AptoSourceList*)sourceList numberOfChildrenOfItem:(id)item
{
  if (item == nil) {
    return [sourceListItems count];
  }
  
  return [[item children] count];
}


- (id) sourceList:(AptoSourceList*)aSourceList child:(NSUInteger)index ofItem:(id)item
{
  if (item == nil) {
    return [sourceListItems objectAtIndex:index];
  }
  
  return [[item children] objectAtIndex:index];
}


- (id) sourceList:(AptoSourceList*)aSourceList objectValueForItem:(id)item
{
  return [item title];
}


- (BOOL) sourceList:(AptoSourceList*)aSourceList isItemExpandable:(id)item
{
  return [item hasChildren];
}


- (void) sourceList:(AptoSourceList*)aSourceList setObjectValue:(id)object forItem:(id)item
{
  [item setTitle:object];
}


- (BOOL) sourceList:(AptoSourceList*)aSourceList itemHasBadge:(id)item
{
  return [item hasBadge];
}


- (NSInteger) sourceList:(AptoSourceList*)aSourceList badgeValueForItem:(id)item
{
  return [item badgeValue];
}


- (BOOL) sourceList:(AptoSourceList*)aSourceList itemHasIcon:(id)item
{
  return [item hasIcon];
}


- (NSImage*) sourceList:(AptoSourceList*)aSourceList iconForItem:(id)item
{
  return [item icon];
}


// AptoSourceList Drag and Drop
// --------------------------------------------------------------------------------------------------------------
#pragma mark - AptoSourceList Drag and Drop

- (BOOL) sourceList:(AptoSourceList*)aSourceList writeItems:(NSArray*)items toPasteboard:(NSPasteboard*)pboard
{
  return NO;
}


- (NSDragOperation) sourceList:(AptoSourceList*)sourceList validateDrop:(id<NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index
{
  return NSDragOperationNone;
}


- (BOOL) sourceList:(AptoSourceList*)AptoSourceList acceptDrop:(id<NSDraggingInfo>)info item:(id)item childIndex:(NSInteger)index
{
  return NO;
}


// --------------------------------------------------------------------------------------------------------------
@end
