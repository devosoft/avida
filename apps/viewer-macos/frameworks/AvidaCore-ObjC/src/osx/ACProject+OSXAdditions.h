//
//  ACProject+OSXAdditions.h
//  avida/apps/viewer-macos/frameworks/AvidaCore-OSX
//
//  Created by David M. Bryson on 3/19/13.
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

#import <AvidaCore/ACProject.h>
#import <Apto/Apto.h>

@interface ACProject (OSXAdditions) <AptoSourceListDataSource>

// AptoSourceListDataSource
// --------------------------------------------------------------------------------------------------------------
#pragma mark - AptoSourceListDataSource

- (NSUInteger) sourceList:(AptoSourceList*)sourceList numberOfChildrenOfItem:(id)item;
- (id) sourceList:(AptoSourceList*)aSourceList child:(NSUInteger)index ofItem:(id)item;
- (id) sourceList:(AptoSourceList*)aSourceList objectValueForItem:(id)item;
- (BOOL) sourceList:(AptoSourceList*)aSourceList isItemExpandable:(id)item;

- (void) sourceList:(AptoSourceList*)aSourceList setObjectValue:(id)object forItem:(id)item;

- (BOOL) sourceList:(AptoSourceList*)aSourceList itemHasBadge:(id)item;
- (NSInteger) sourceList:(AptoSourceList*)aSourceList badgeValueForItem:(id)item;

- (BOOL) sourceList:(AptoSourceList*)aSourceList itemHasIcon:(id)item;
- (NSImage*) sourceList:(AptoSourceList*)aSourceList iconForItem:(id)item;


- (BOOL) sourceList:(AptoSourceList*)aSourceList writeItems:(NSArray*)items toPasteboard:(NSPasteboard*)pboard;
- (NSDragOperation) sourceList:(AptoSourceList*)sourceList validateDrop:(id<NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index;
- (BOOL) sourceList:(AptoSourceList*)AptoSourceList acceptDrop:(id<NSDraggingInfo>)info item:(id)item childIndex:(NSInteger)index;

- (id<NSPasteboardWriting>) sourceList:(AptoSourceList*)aSourceList pasteboardWriterForItem:(id)item;


// --------------------------------------------------------------------------------------------------------------
@end
