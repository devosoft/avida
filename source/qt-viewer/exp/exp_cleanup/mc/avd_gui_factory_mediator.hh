#ifndef AVD_GUI_FACTORY_MEDIATOR_HH
#define AVD_GUI_FACTORY_MEDIATOR_HH

#ifndef QOBJECT_H
#include <qobject.h>
#endif

class avdGUIFactoryMediator : public QObject {
  Q_OBJECT
public:
  avdGUIFactoryMediator(
    QObject *parent = 0, const char *name = "(avdGUIFactoryMediator)")
  :QObject(parent, name){}
signals:
  void newGUIControllerSig(const QString &);
};

#endif

// arch-tag: header file for gui factory mediator
