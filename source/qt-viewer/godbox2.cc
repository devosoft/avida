/****************************************************************************
** Form implementation generated from reading ui file '../../../current/source/qt-viewer/godbox.ui'
**
** Created: Wed Mar 20 09:33:29 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "godbox2.hh"

#include <qvariant.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>


using namespace std;


/* 
 *  Constructs a GodBox2 which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
GodBox2::GodBox2( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "GodBox2" );
    resize( 564, 280 ); 
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 490, 256 ) );
    setMaximumSize( QSize( 32767, 32767 ) );
    setCaption( trUtf8( "Play God" ) );
    GodBox2Layout = new QVBoxLayout( this, 11, 6, "GodBox2Layout"); 

    Layout34 = new QHBoxLayout( 0, 0, 6, "Layout34"); 

    GroupBox7 = new QGroupBox( this, "GroupBox7" );
    GroupBox7->setTitle( trUtf8( "Mutation Rates" ) );

    QWidget* privateLayoutWidget = new QWidget( GroupBox7, "Layout33" );
    privateLayoutWidget->setGeometry( QRect( 11, 21, 287, 180 ) ); 
    Layout33 = new QGridLayout( privateLayoutWidget, 1, 1, 0, 6, "Layout33"); 

    Layout28 = new QHBoxLayout( 0, 0, 6, "Layout28"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout28->addItem( spacer );

    m_ins_mut_slider = new QSlider( privateLayoutWidget, "m_ins_mut_slider" );
    m_ins_mut_slider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, m_ins_mut_slider->sizePolicy().hasHeightForWidth() ) );
    m_ins_mut_slider->setOrientation( QSlider::Vertical );
    Layout28->addWidget( m_ins_mut_slider );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout28->addItem( spacer_2 );

    Layout33->addLayout( Layout28, 2, 1 );

    Layout29 = new QHBoxLayout( 0, 0, 6, "Layout29"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout29->addItem( spacer_3 );

    m_cp_mut_slider = new QSlider( privateLayoutWidget, "m_cp_mut_slider" );
    m_cp_mut_slider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, m_cp_mut_slider->sizePolicy().hasHeightForWidth() ) );
    m_cp_mut_slider->setOrientation( QSlider::Vertical );
    Layout29->addWidget( m_cp_mut_slider );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout29->addItem( spacer_4 );

    Layout33->addLayout( Layout29, 2, 2 );

    m_cp_mut_lineedit = new QLineEdit( privateLayoutWidget, "m_cp_mut_lineedit" );

    Layout33->addWidget( m_cp_mut_lineedit, 1, 2 );

    TextLabel2_3 = new QLabel( privateLayoutWidget, "TextLabel2_3" );
    TextLabel2_3->setText( trUtf8( "Insert/Delete:" ) );

    Layout33->addWidget( TextLabel2_3, 0, 1 );

    TextLabel2_4 = new QLabel( privateLayoutWidget, "TextLabel2_4" );
    TextLabel2_4->setText( trUtf8( "Copy:" ) );

    Layout33->addWidget( TextLabel2_4, 0, 2 );

    Layout27 = new QHBoxLayout( 0, 0, 6, "Layout27"); 
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout27->addItem( spacer_5 );

    m_pt_mut_slider = new QSlider( privateLayoutWidget, "m_pt_mut_slider" );
    m_pt_mut_slider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, m_pt_mut_slider->sizePolicy().hasHeightForWidth() ) );
    m_pt_mut_slider->setOrientation( QSlider::Vertical );
    Layout27->addWidget( m_pt_mut_slider );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout27->addItem( spacer_6 );

    Layout33->addLayout( Layout27, 2, 0 );

    m_ins_mut_lineedit = new QLineEdit( privateLayoutWidget, "m_ins_mut_lineedit" );

    Layout33->addWidget( m_ins_mut_lineedit, 1, 1 );

    m_pt_mut_lineedit = new QLineEdit( privateLayoutWidget, "m_pt_mut_lineedit" );

    Layout33->addWidget( m_pt_mut_lineedit, 1, 0 );

    TextLabel2 = new QLabel( privateLayoutWidget, "TextLabel2" );
    TextLabel2->setText( trUtf8( "Cosmic Rays:" ) );

    Layout33->addWidget( TextLabel2, 0, 0 );
    Layout34->addWidget( GroupBox7 );

    GroupBox1_4 = new QGroupBox( this, "GroupBox1_4" );
    GroupBox1_4->setTitle( trUtf8( "Apocalypse" ) );

    QWidget* privateLayoutWidget_2 = new QWidget( GroupBox1_4, "Layout32" );
    privateLayoutWidget_2->setGeometry( QRect( 11, 21, 93, 180 ) ); 
    Layout32 = new QVBoxLayout( privateLayoutWidget_2, 0, 6, "Layout32"); 

    TextLabel2_5 = new QLabel( privateLayoutWidget_2, "TextLabel2_5" );
    TextLabel2_5->setText( trUtf8( "Death Rate:" ) );
    Layout32->addWidget( TextLabel2_5 );

    m_apocalypse_lineedit = new QLineEdit( privateLayoutWidget_2, "m_apocalypse_lineedit" );
    Layout32->addWidget( m_apocalypse_lineedit );

    Layout30 = new QHBoxLayout( 0, 0, 6, "Layout30"); 
    QSpacerItem* spacer_7 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout30->addItem( spacer_7 );

    m_apocalypse_slider = new QSlider( privateLayoutWidget_2, "m_apocalypse_slider" );
    m_apocalypse_slider->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, m_apocalypse_slider->sizePolicy().hasHeightForWidth() ) );
    m_apocalypse_slider->setOrientation( QSlider::Vertical );
    Layout30->addWidget( m_apocalypse_slider );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout30->addItem( spacer_8 );
    Layout32->addLayout( Layout30 );

    m_apocalypse_pushbutton = new QPushButton( privateLayoutWidget_2, "m_apocalypse_pushbutton" );
    m_apocalypse_pushbutton->setText( trUtf8( "Kill Creatures" ) );
    Layout32->addWidget( m_apocalypse_pushbutton );
    Layout34->addWidget( GroupBox1_4 );

    GroupBox1_5 = new QGroupBox( this, "GroupBox1_5" );
    GroupBox1_5->setTitle( trUtf8( "Repopulate" ) );

    QWidget* privateLayoutWidget_3 = new QWidget( GroupBox1_5, "Layout31" );
    privateLayoutWidget_3->setGeometry( QRect( 10, 21, 84, 180 ) ); 
    Layout31 = new QVBoxLayout( privateLayoutWidget_3, 0, 6, "Layout31"); 

    TextLabel1_6 = new QLabel( privateLayoutWidget_3, "TextLabel1_6" );
    TextLabel1_6->setText( trUtf8( "New Creature Type:" ) );
    Layout31->addWidget( TextLabel1_6 );

    m_inject_creature_combobox = new QComboBox( FALSE, privateLayoutWidget_3, "m_inject_creature_combobox" );
    Layout31->addWidget( m_inject_creature_combobox );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout31->addItem( spacer_9 );

    m_inject_one_button = new QPushButton( privateLayoutWidget_3, "m_inject_one_button" );
    m_inject_one_button->setText( trUtf8( "Inject One Creature" ) );
    Layout31->addWidget( m_inject_one_button );

    m_inject_all_button = new QPushButton( privateLayoutWidget_3, "m_inject_all_button" );
    m_inject_all_button->setText( trUtf8( "Replace Population" ) );
    Layout31->addWidget( m_inject_all_button );
    Layout34->addWidget( GroupBox1_5 );
    GodBox2Layout->addLayout( Layout34 );

    Layout35 = new QHBoxLayout( 0, 0, 6, "Layout35"); 

    m_help_button = new QPushButton( this, "m_help_button" );
    m_help_button->setText( trUtf8( "Help" ) );
    Layout35->addWidget( m_help_button );
    QSpacerItem* spacer_10 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout35->addItem( spacer_10 );

    m_close_button = new QPushButton( this, "m_close_button" );
    m_close_button->setText( trUtf8( "Close" ) );
    Layout35->addWidget( m_close_button );
    GodBox2Layout->addLayout( Layout35 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
GodBox2::~GodBox2()
{
    // no need to delete child widgets, Qt does it all for us
}

