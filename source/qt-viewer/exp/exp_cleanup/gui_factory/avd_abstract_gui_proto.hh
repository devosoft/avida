#ifndef AVD_ABSTRACT_GUI_PROTO_HH
#define AVD_ABSTRACT_GUI_PROTO_HH

class avdMCMediator;
class avdAbstractCtrl : public cRetainable
{ public: virtual bool setup(avdMCMediator *mediator) = 0; };

#endif

// arch-tag: proto file for generic gui object
