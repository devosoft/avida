//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <qpixmap.h>
#include <qcolor.h>
#include <qpainter.h>

#include "n_orig_instruction_pixmapstructs.hh"


void
PixmapHlSet::drawPrimitive(
  QPixmap *pixmap,
  int x, int y, int width, int height, int spacing,
  const QColor &color,
  const QString &string, int text_flags
){
  pixmap->resize(width, height);
  pixmap->setOptimization(QPixmap::BestOptim);
  QPainter p(pixmap);
  p.fillRect(x, y, width, height, color);
  p.drawText(
    x + (2 * spacing),
    y + (2 * spacing),
    width - (4 * spacing),
    height - (4 * spacing),
    text_flags, string
  );
}

void
PixmapHlSet::deletePixmaps(void){
  delete m_pixmaps[NO_HL];
  delete m_pixmaps[IH_HL];
  delete m_pixmaps[RH_HL];
  delete m_pixmaps[WH_HL];
  delete m_pixmaps[FH_HL];
}

void
PixmapHlSet::buildPixmapSet(
  int x, int y, int width, int height, int spacing,
    const QColor &uhl_color,
    const QColor &ih_hl_color,
    const QColor &rh_hl_color,
    const QColor &wh_hl_color,
    const QColor &fh_hl_color,
  const QString &string, int text_flags
){
  if(m_initialized){
    deletePixmaps();
  }
  m_initialized = true;
  m_pixmaps[NO_HL] = new QPixmap();
  drawPrimitive(
    m_pixmaps[NO_HL],
    x, y, width, height, spacing,
    uhl_color,
    string, text_flags
  );
  m_pixmaps[IH_HL] = new QPixmap();
  drawPrimitive(
    m_pixmaps[IH_HL],
    x, y, width, height, spacing,
    ih_hl_color,
    string, text_flags
  );
  m_pixmaps[RH_HL] = new QPixmap();
  drawPrimitive(
    m_pixmaps[RH_HL],
    x, y, width, height, spacing,
    rh_hl_color,
    string, text_flags
  );
  m_pixmaps[WH_HL] = new QPixmap();
  drawPrimitive(
    m_pixmaps[WH_HL],
    x, y, width, height, spacing,
    wh_hl_color,
    string, text_flags
  );
  m_pixmaps[FH_HL] = new QPixmap();
  drawPrimitive(
    m_pixmaps[FH_HL],
    x, y, width, height, spacing,
    fh_hl_color,
    string, text_flags
  );
}

