//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "n_orig_instruction_field_widget.hh"


N_Instruction_FieldWidget::N_Instruction_FieldWidget(
  QWidget *parent,
  const char *name,
  WFlags f
)
: QLabel(parent, name, f)
{
  ;
}

N_Instruction_FieldWidget::N_Instruction_FieldWidget(
  const QString &text,
  QWidget *parent,
  const char *name,
  WFlags f
)
: QLabel(text, parent, name, f)
{
  ;
}

void
N_Instruction_FieldWidget::enterEvent(QEvent *){
  setFrameStyle(QFrame::Panel);
}

void
N_Instruction_FieldWidget::leaveEvent(QEvent *){
  setFrameStyle(QFrame::NoFrame);
}

void
N_Instruction_FieldWidget::mousePressEvent(QMouseEvent *e){
  emit pressed(mapToGlobal(QPoint(0, height())));
}
