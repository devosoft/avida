/****************************************************************************
** Form interface generated from reading ui file '../../../current/source/qt-viewer/godbox.ui'
**
** Created: Wed Mar 20 09:33:19 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef GODBOX2_H
#define GODBOX2_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSlider;

class GodBox2 : public QWidget
{ 
    Q_OBJECT

public:
    GodBox2( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~GodBox2();

    QGroupBox* GroupBox7;
    QSlider* m_ins_mut_slider;
    QSlider* m_cp_mut_slider;
    QLineEdit* m_cp_mut_lineedit;
    QLabel* TextLabel2_3;
    QLabel* TextLabel2_4;
    QSlider* m_pt_mut_slider;
    QLineEdit* m_ins_mut_lineedit;
    QLineEdit* m_pt_mut_lineedit;
    QLabel* TextLabel2;
    QGroupBox* GroupBox1_4;
    QLabel* TextLabel2_5;
    QLineEdit* m_apocalypse_lineedit;
    QSlider* m_apocalypse_slider;
    QPushButton* m_apocalypse_pushbutton;
    QGroupBox* GroupBox1_5;
    QLabel* TextLabel1_6;
    QComboBox* m_inject_creature_combobox;
    QPushButton* m_inject_one_button;
    QPushButton* m_inject_all_button;
    QPushButton* m_help_button;
    QPushButton* m_close_button;


protected:
    QVBoxLayout* GodBox2Layout;
    QHBoxLayout* Layout34;
    QGridLayout* Layout33;
    QHBoxLayout* Layout28;
    QHBoxLayout* Layout29;
    QHBoxLayout* Layout27;
    QVBoxLayout* Layout32;
    QHBoxLayout* Layout30;
    QVBoxLayout* Layout31;
    QHBoxLayout* Layout35;
};

#endif // GODBOX2_H
