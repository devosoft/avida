//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef N_ORIG_INSTRUCTION_PIXMAPSTRUCTS_HH
#define N_ORIG_INSTRUCTION_PIXMAPSTRUCTS_HH


class QPixmap;
class QColor;
class QString;

class PixmapHlSet {
private:
  bool m_initialized;
  void deletePixmaps(void);
public:
  enum HighlightEnum {NO_HL, IH_HL, RH_HL, WH_HL, FH_HL, COUNT_HL};
  QPixmap *uhl, *ih_hl;
  QPixmap *m_pixmaps[COUNT_HL];
  PixmapHlSet(void){ m_initialized = false; }
  ~PixmapHlSet(void){ if(m_initialized) deletePixmaps(); }
  void drawPrimitive(
    QPixmap *pixmap,
    int x, int y, int width, int height, int spacing,
    const QColor &color,
    const QString &string, int text_flags
  );
  void
  buildPixmapSet(
    int x, int y, int width, int height, int spacing,
    const QColor &uhl_color,
    const QColor &ih_hl_color,
    const QColor &rh_hl_color,
    const QColor &wh_hl_color,
    const QColor &fh_hl_color,
    const QString &string, int text_flags
  );
};
class PixmapOnOffSet {
public:
  PixmapHlSet on, off;
};


#endif /* !N_ORIG_INSTRUCTION_PIXMAPSTRUCTS_HH */
