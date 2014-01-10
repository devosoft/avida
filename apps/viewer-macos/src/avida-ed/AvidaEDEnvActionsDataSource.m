//
//  AvidaEDEnvActionsDataSource.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 3/6/12.
//  Copyright 2012 Michigan State University. All rights reserved.
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

#import "AvidaEDEnvActionsDataSource.h"


@interface NSDictionary (AvidaEDPopViewStatViewEnvActionsSorting) 
-(NSComparisonResult)compareEnvActionOrder:(NSDictionary*)row;
@end

@implementation NSDictionary (AvidaEDPopViewStatViewEnvActionsSorting)
-(NSComparisonResult)compareEnvActionOrder:(NSDictionary*)row;
{
  NSNumber* rowOrder = [row objectForKey:@"Order"];
  NSNumber* selfOrder = [self objectForKey:@"Order"];
  
  return [selfOrder compare:rowOrder];
}
@end



@implementation AvidaEDEnvActionsDataSource

- (id) init {
entries = [[NSMutableArray alloc] init];
entrymap = [[NSMutableDictionary alloc] init];
return self;
}

- (void) addNewEntry:(NSString*)name withDescription:(NSString*)desc withOrder:(int)order {
  NSMutableDictionary* entry = [[NSMutableDictionary alloc] init];
  [entry setValue:name forKey:@"Action"];
  [entry setValue:[NSString stringWithFormat:@"%@-", name] forKey:@"AttributedAction"];
  [entry setValue:[NSNumber numberWithInt:NSOffState] forKey:@"State"];
  [entry setValue:desc forKey:@"Description"];
  [entry setValue:[NSNumber numberWithInt:0] forKey:@"Orgs Performing"];
  [entry setValue:[NSNumber numberWithInt:order] forKey:@"Order"];
  
  [entries addObject:entry];

  // Sort and update entry map
  [entries sortUsingSelector:@selector(compareEnvActionOrder:)];
  for (NSUInteger i = 0; i < [entries count]; i++) {
    [entrymap setValue:[NSNumber numberWithLong:i] forKey:[[entries objectAtIndex:i] valueForKey:@"Action"]];
  }
}


- (void) updateEntry:(NSString*)name withValue:(NSNumber*)value {
  NSMutableDictionary* entry = [entries objectAtIndex:[[entrymap valueForKey:name] unsignedIntValue]];
  [entry setValue:value forKey:@"Orgs Performing"];
  char attr = [value intValue] > 0 ? '+' : '-';
  [entry setValue:[NSString stringWithFormat:@"%@%c", name, attr] forKey:@"AttributedAction"];
}


- (void) clearEntries {
  [entries removeAllObjects];
}

- (NSString*) entryAtIndex:(NSUInteger)idx {
  return [[entries objectAtIndex:idx] valueForKey:@"Action"];
}

- (NSUInteger) entryCount {
  return [entries count];
}

- (id) valueOfEntry:(NSString*)entry forKey:(NSString*)key {
  return [[entries objectAtIndex:[[entrymap valueForKey:entry] unsignedIntValue]] valueForKey:key];
}



- (NSInteger) numberOfRowsInTableView:(NSTableView*)tableView {
  return [entries count];
}

- (id) tableView:(NSTableView*)tableView objectValueForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex {
  id entry, value;
  
  entry = [entries objectAtIndex:rowIndex];
  value = [entry objectForKey:[tableColumn identifier]];
  return value;
}

- (void) tableView:(NSTableView*)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)rowIndex;
{
  if ([[tableColumn identifier] isEqualToString:@"State"]) {
    id entry = [entries objectAtIndex:rowIndex];
    [entry setValue:object forKey:@"State"];
    if ([[tableView delegate] respondsToSelector:@selector(envActionStateChange:)]) {
      NSMutableDictionary* states = [[NSMutableDictionary alloc] init];
      for (int i = 0; i < [entries count]; i++) {
        NSMutableDictionary* cur_entry = [entries objectAtIndex:i];
        [states setValue:[cur_entry valueForKey:@"State"] forKey:[cur_entry valueForKey:@"Action"]];
      }
      [[tableView delegate] performSelector:@selector(envActionStateChange:) withObject:states];
    }
  }
}

@end
