#ifndef AVD_USER_MSG_CTRL_FUNCT_PROTO_HH
#define AVD_USER_MSG_CTRL_FUNCT_PROTO_HH

class avdUserMsgCtrl;
typedef Loki::Functor<void, TYPELIST_1(const avdUserMsgCtrl &)>
avdUserMsgCtrlFunct;

#endif

// arch-tag: proto file for user debug-message display controller functors
