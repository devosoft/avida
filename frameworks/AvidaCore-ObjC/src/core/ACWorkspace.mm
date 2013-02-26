//
//  ACWorkspace.m
//  avida/apps/viewer-macos/frameworks/AvidaCore-ObjC
//
//  Created by David M. Bryson on 12/21/12.
//  Copyright 2012-2013 Michigan State University. All rights reserved.
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

#import "ACWorkspace.h"

#import "ACFramework.h"
#import "ACProject_Private.h"


// Global Constants
// --------------------------------------------------------------------------------------------------------------
#pragma mark Global Constants

NSString* const ACWorkspaceFiletypes[] = {@"avidaworkspace", @"org.devosoft.avida.avida-workspace"};


// ACWorkspace Private Interface
// --------------------------------------------------------------------------------------------------------------

@interface ACWorkspace ()
- (ACWorkspace*) initWithFreezer:(Avida::Viewer::FreezerPtr)f atURL:(NSURL*)url;
@end


// ACWorkspace Implementations
// --------------------------------------------------------------------------------------------------------------

@implementation ACWorkspace

@synthesize name;
@synthesize location=workspaceURL;
@dynamic detailsString;
@dynamic lastOpenedString;


- (NSString*) detailsString {
  return @"workspace details here...";
}


- (NSString*) lastOpenedString {
  NSError* err = nil;
  NSDictionary* resVals = [workspaceURL resourceValuesForKeys:@[NSURLContentAccessDateKey] error:&err];
  
  NSDate* lastOpenedDate = [resVals objectForKey:NSURLContentAccessDateKey];
  if (lastOpenedDate) {
    NSDateFormatter* dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setTimeStyle:NSDateFormatterNoStyle];
    [dateFormatter setDateStyle:NSDateFormatterShortStyle];
    [dateFormatter setDoesRelativeDateFormatting:YES];
    NSString* dateString = [dateFormatter stringFromDate:lastOpenedDate];
    
    return dateString;
  }
  
  return @"unknown";
}


// Static Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Static Initialization

+ (void) initialize {
  if (self == [ACWorkspace class]) {
    [ACFramework self];
  }
}


// Initialization and Creation
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Initialization and Creation

- (ACWorkspace*) initWithURL:(NSURL*)url {
  NSError* err = nil;
  if (![url isFileURL] || ![url checkResourceIsReachableAndReturnError:&err]) return nil;
  
  self = [super init];
  
  if (self) {
    Avida::Viewer::FreezerPtr f = Avida::Viewer::Freezer::LoadWithPath([[url path] UTF8String]);
    if (!f) return nil;
    
    workspaceURL = [url fileReferenceURL];
    freezer = f;
    name = [[workspaceURL lastPathComponent] stringByDeletingPathExtension];
  }
  
  return self;
}


- (ACWorkspace*) initWithFreezer:(Avida::Viewer::FreezerPtr)f atURL:(NSURL*)url {
  self = [super init];
  
  if (self) {
    workspaceURL = [url fileReferenceURL];
    freezer = f;
    name = [[workspaceURL lastPathComponent] stringByDeletingPathExtension];
  }
  
  return self;
}


+ (ACWorkspace*) createAtURL:(NSURL*)url {
  if (![url isFileURL]) return nil;
  
  Avida::Viewer::FreezerPtr f = Avida::Viewer::Freezer::CreateWithPath([[url path] UTF8String]);
  if (!f) return nil;
  
  return [[ACWorkspace alloc] initWithFreezer:f atURL:url];
}


// Project Listing and Access
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Project Listing and Access

- (ACProject*) projectWithName:(NSString*)name
{
  // Return the default project for now
  return [ACProject projectWithFreezer:freezer];
}


// Global Information
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Global Information

+ (NSArray*) fileTypes {
  static NSArray* workspaceFiletypes = nil;
  if (workspaceFiletypes == nil) {
    workspaceFiletypes = [[NSArray alloc] initWithObjects:ACWorkspaceFiletypes count:2];
  }
  
  return workspaceFiletypes;
}


// --------------------------------------------------------------------------------------------------------------
@end
