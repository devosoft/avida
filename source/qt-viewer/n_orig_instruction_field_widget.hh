//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_FIELD_WIDGET_HH
#define N_ORIG_INSTRUCTION_FIELD_WIDGET_HH

#include <qlabel.h>

class N_Instruction_FieldWidget : public QLabel {
  Q_OBJECT
private:
public:
  N_Instruction_FieldWidget(
    QWidget *parent,
    const char *name = 0,
    WFlags f = 0
  );
  N_Instruction_FieldWidget(
    const QString &text,
    QWidget *parent,
    const char *name = 0,
    WFlags f = 0
  );
signals:
  void pressed(const QPoint &);
private:
  virtual void enterEvent(QEvent *);
  virtual void leaveEvent(QEvent *);
  virtual void mousePressEvent(QMouseEvent *e);
};

#endif /* !N_ORIG_INSTRUCTION_FIELD_WIDGET_HH */
