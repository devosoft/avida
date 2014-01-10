//
//  WorkspaceViewController.m
//  avida/apps/viewer-macos
//
//  Created by David M. Bryson on 12/21/12.
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

#import "WorkspaceViewController.h"

#import "WorkspaceProjectDetailViewController.h"
#import "WorkspaceProjectSelectViewController.h"


@interface WorkspaceViewController ()

@end


// WorkspaceViewController Implementation
// --------------------------------------------------------------------------------------------------------------

@implementation WorkspaceViewController


// Properties
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Properties

@synthesize workspace;
@synthesize view;
@synthesize delegate;


// Initialization
// --------------------------------------------------------------------------------------------------------------
#pragma mark - Initialization

- (WorkspaceViewController*) initWithWorkspace:(ACWorkspace*)ws delegate:(id<WorkspaceViewDelegate>)d;
{
  self = [super init];
  if (self) {
    workspace = ws;
    
    detailViewCtlr = [[WorkspaceProjectDetailViewController alloc] initWithWorkspaceView:self];
    selectViewCtlr = [[WorkspaceProjectSelectViewController alloc] initWithWorkspaceView:self];
    
    delegate = d;
    view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 200, 400)];
    [view setTranslatesAutoresizingMaskIntoConstraints:NO];
    
    [view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:[view(>=200)]" options:0 metrics:nil views:NSDictionaryOfVariableBindings(view)]];
    [view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:[view(>=200)]" options:0 metrics:nil views:NSDictionaryOfVariableBindings(view)]];
    
    NSView* detailView = detailViewCtlr.view;
    [detailView setTranslatesAutoresizingMaskIntoConstraints:NO];
    [view addSubview:detailView];
    [view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|-0-[detailView]-0-|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(detailView)]];
    [view addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|-0-[detailView]-0-|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(detailView)]];
  }
    
  return self;
}


// --------------------------------------------------------------------------------------------------------------
@end
