//
//  XGSServer.h
//  GridStuffer
//
//  Created by Charles Parnot on 5/18/05.
//  Copyright 2005 Charles Parnot. All rights reserved.
//

/*
 This file is part of GridStuffer.
 GridStuffer is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 GridStuffer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with GridStuffer; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
The XGSServer instances are managed objects that expose a simple interface for Xgrid Controllers. Behind the scenes, this class uses a number of other private classes for the server connections, the creation of an application-wide shared managed object context and browsing the local network for servers advertising their services.

Because of all these things happening behind the scenes, instances of XGSServer should only be created/retrieved using the public methods listed here. There are three ways to create/retrieve XGSServer instances:
 - using the class method '+allServers'. The returned array contain servers already connected, but also servers that were connected to in previous sessions (saved in a persistent store) and servers found on the local network (if browsing was started using the 'startBrowsing').
 - using the class method '+serverWithAddress:', which may create a new instance or retrieve an existing record from the shared managed object context created by the framework
 - using the class method '+serverWithAddress:managedObjectContext:', which you can use to create and retrieve server objects in a custom managed object context; to "copy" an existing XGSServer into a different managed object context, you can use '-serverInManagedObjectContext:'
 
These methods ensure that only one instance of XGSServer exists per server address and per managed object context. Once retrieved, the returned instance can be retained as long as needed, and will remain valid and in sync until released. However, it is possible that several instances with the same server address may exist in different managed object contexts. This is fine: connection and network traffic will not be duplicated. The connection process itself is shared by all XGSServer instances with the same address, and these instances are guaranteed to be kept in sync all the time.

To start the connection, call one of the -connect... method.
Then use a delegate or notifications to keep track of the connection status asynchronously.

IMPORTANT: in the current implementation, the password will NOT be saved to disk.
*/

//Constants to use to subscribe to notifications
APPKIT_EXTERN NSString *XGSServerDidConnectNotification;
APPKIT_EXTERN NSString *XGSServerDidLoadNotification;
APPKIT_EXTERN NSString *XGSServerDidNotConnectNotification;
APPKIT_EXTERN NSString *XGSServerDidDisconnectNotification;

@class XGSServerConnection;
@class XGSGrid;

@interface XGSServer : XGSManagedObject
{
	XGSServerConnection *serverConnection;
	id delegate;
}

// The browser methods will automatically detect servers advertising on the Bonjour network. These will be asynchronously added to the list returned by allServers
+ (void)startBrowsing;
+ (void)stopBrowsing;

// This is the recommanded way to retrieve servers (which might also trigger the creation of a new server if it does not exist yet)
+ (NSArray *)allServers;
+ (XGSServer *)serverWithAddress:(NSString *)address;

// The XGSServer instances returned by the above methods live in a managed object context set up by the framework, and this context is unique for the whole application. If you want to use XGSServer objects in a custom managed object context, do not create them yourself but use one of the methods below. These methods will set up the connection properly and they guarantee that there is only one XGSServer object per server address and per context.
+ (XGSServer *)serverWithAddress:(NSString *)address inManagedObjectContext:(NSManagedObjectContext *)context;
- (XGSServer *)serverInManagedObjectContext:(NSManagedObjectContext *)context;

//accessors
- (NSString *)address;
- (XGController *)xgridController;
- (XGSGrid *)defaultGrid;

//see below, the informal protocol for the delegate and notifications
- (id)delegate;
- (void)setDelegate:(id)newDelegate;

//after calling these methods, use the delegate or the notifications to be notified asynchronously of the connection results
- (void)connectWithoutAuthentication;
- (void)connectWithSingleSignOnCredentials;
- (void)connectWithPassword:(NSString *)password;
- (void)disconnect;

//These keys can be observed with KVO
//To get a notification that the connection is ready, observe isConnecting until NO, then check if isConnected is YES
- (BOOL)isAvailable;
- (BOOL)isConnected;
- (BOOL)isConnecting;
- (NSString *)statusString;

@end

//methods that can be implemented by the delegate
@interface NSObject (XGSServerDelegate)
- (void)serverDidConnect:(XGSServer *)aServer;
- (void)serverDidNotConnect:(XGSServer *)aServer;
- (void)serverDidDisconnect:(XGSServer *)aServer;
//- (void)server:(XGSServer *)aServer didAddGrid:(XGSGrid *)aGrid;
//- (void)server:(XGSServer *)aServer didRemoveGrid:(XGSGrid *)aGrid;
@end
