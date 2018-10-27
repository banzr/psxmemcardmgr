/****************************************************************
**
** Qt tutorial 6
**
****************************************************************/
#include <qapplication.h>

#include <qpushbutton.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qfont.h>
#include <QAction>

#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <QMenu>
#include <qtoolbar.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <QProgressDialog>

#include "CardSlots.h"
#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    cardslots = new CardSlots(this);  // Create the card widget
	psx_card = new PSX_memory_card();  // Create a memory card

	/****************** Set up the menu bar ********************/
    QMenu *mnuFile = new QMenu(this);
    QMenu *mnuEdit = new QMenu(this);
    QMenu *mnuMemCard = new QMenu(this);
    QMenu *mnuModify = new QMenu(this);

    QMenu *mnuHelp = new QMenu(this);


    menuBar()->insertMenu(new QAction("&File",this), mnuFile);
        mnuFile->insertMenu(new QAction( "&New Memory Card Image",this), this, SLOT( new_mc_image() ),Qt::CTRL+Qt::Key_N);
        mnuFile->insertSeparator(new QAction());
        mnuFile->insertMenu(new QAction( "&Load Image",this), this, SLOT( load_mc_image() ),Qt::CTRL+Qt::Key_O);
        mnuFile->insertMenu(new QAction( "&Save Image",this), this, SLOT( save_mc_image() ),Qt::CTRL+Qt::Key_S);
        mnuFile->insertSeparator(new QAction());
        mnuFile->insertMenu(new QAction( "Load Single Game Save",this), this, SLOT( load_single_game() ));
        mnuFile->insertMenu(new QAction( "Save Single Game Save",this), this, SLOT( save_single_game() ));
        mnuFile->insertSeparator(new QAction());
        mnuFile->insertMenu(new QAction( "&Quit",this), qApp, SLOT( quit() ) );

    menuBar()->insertMenu(new QAction("&Edit",this), mnuEdit);
        mnuEdit->insertMenu(new QAction("Cut",this), this, SLOT( dummy() ) );
        mnuEdit->insertMenu(new QAction("Copy",this), this, SLOT( dummy() ) );
        mnuEdit->insertMenu(new QAction("Paste",this), this, SLOT( dummy() ) );
        mnuEdit->insertSeparator(new QAction());
        mnuEdit->insertMenu(new QAction("Delete block",this), this, SLOT( del() ) );
        mnuEdit->insertMenu(new QAction("UnDelete block",this), this, SLOT( undel() ) );

    menuBar()->insertMenu(new QAction("&Modify", mnuModify);
        mnuModify->insertMenu(new QAction("Edit Product code...",this), this, SLOT( set_gamePcode() ) );
        mnuModify->insertMenu(new QAction("Edit Game ID...",this), this, SLOT( set_gameID() ) );
        mnuModify->insertMenu(new QAction("Edit Title...",this), this, SLOT( dummy() ) );

    menuBar()->insertMenu(new QAction("&MemCard",this), mnuMemCard);
        mnuMemCard->insertMenu(new QAction( "Write to memory card",this), this, SLOT( upload() ) );
        mnuMemCard->insertMenu(new QAction( "Read from memory card",this), this, SLOT( download() ) );


    menuBar()->insertMenu(new QAction("&Help",this), mnuHelp);
        mnuHelp->insertMenu(new QAction("&About...",this), this, SLOT( showabout() ));
        mnuHelp->insertMenu(new QAction("&About Qt...",this), this, SLOT( showaboutQt() ));


	setCentralWidget(cardslots);
    statusBar()->showMessage( "Ready", 2000 );
	resize( 400, 340 );

    clear();   // just in case
}

void MainWindow::new_mc_image()
{
	clear();
}

void MainWindow::load_mc_image()
{
    QString fn = QFileDialog::getOpenFileName(this, QString("Load memory card image"), QString(), "Memory Card images(*.mc *.mcr *.gme)");
    if ( !fn.isEmpty() )
        load( fn );
    else
        statusBar()->showMessage( "Loading aborted", 2000 );
}


void MainWindow::save_mc_image()
{
    QString fn = QFileDialog::getSaveFileName(this, QString("Save memory card image"), QString(), "*.mc");
    if ( !fn.isEmpty() )
        save( fn );
    else
        statusBar()->showMessage( "Saving aborted", 2000 );
}

void MainWindow::save_single_game()
{
	int selected_slot = cardslots->get_selected_slot();

    if (selected_slot==-1)
	{
		QMessageBox::information(this,"Error","Please select a block first.");
		return;
	}
	if (psx_card->get_slot_is_free(selected_slot))
	{
		QMessageBox::information(this,"Error","Cannot save an empty block");
		return;
	}


    QString fn = QFileDialog::getSaveFileName(this, QString("Save single game"), QString(), "*.mcs");
	if ( !fn.isEmpty() )
        psx_card->save_single_game(fn, selected_slot);
    else
        statusBar()->showMessage( "Saving aborted", 2000 );


}

void MainWindow::load_single_game()
{
	int selected_slot = cardslots->get_selected_slot();

	if (selected_slot==-1)
	{
		QMessageBox::information(this,"Error","Please select a block first.");
		return;
	}
	if (!psx_card->get_slot_is_free(selected_slot))
	{
		if (QMessageBox::warning(this, "Warning","This will overwrite current block. Continue?","Yes","No",0,0,1)==1)
		{
			return;
		}

	}

    QString fn = QFileDialog::getOpenFileName(this, QString("Load single game"), QString(), "*.mcs");
	if ( !fn.isEmpty() )
	{
        psx_card->load_single_game(fn, selected_slot);
		updateSlots();
	}
    else
        statusBar()->showMessage( "Loading aborted", 2000 );

}

void MainWindow::set_gameID()
{
    bool ok = false;
	int selected_slot = cardslots->get_selected_slot();
	if (selected_slot==-1)
	{
		QMessageBox::information(this,"Error","Please select a block first.");
		return;
	}

	QString oldtext = psx_card->get_slot_gameID(selected_slot);
	QString text = QInputDialog::getText(this, "Edit game ID","Please enter the new game ID.", QLineEdit::Normal, oldtext , &ok);
	if ( ok && !text.isEmpty() )
    {
		psx_card->set_slot_gameID(selected_slot, text);
		updateSlots();
	}
	else
        statusBar()->showMessage("Change game ID aborted", 2000);

}

void MainWindow::set_gamePcode()
{
    bool ok = false;
	int selected_slot = cardslots->get_selected_slot();
	if (selected_slot==-1)
	{
		QMessageBox::information(this,"Error","Please select a block first.");
		return;
	}

	QString oldtext = psx_card->get_slot_Pcode(selected_slot);
	QString text = QInputDialog::getText(this, "Edit game Product Code","Please enter the new product code.", QLineEdit::Normal, oldtext , &ok );
	if ( ok && !text.isEmpty() )
    {
		psx_card->set_slot_Pcode(selected_slot, text);
		updateSlots();
	}
	else
        statusBar()->showMessage("Change game ID aborted", 2000);

}

void MainWindow::del()
{
	int selected_slot = cardslots->get_selected_slot();

	if (selected_slot==-1)
	{
		QMessageBox::information(this,"Error","Please select a block first.");
		return;
	}

	if (psx_card->get_slot_is_free(selected_slot))
	{
		QMessageBox::information(this,"Error","This block is already deleted.");
		return;
	}

	psx_card->delete_slot(selected_slot);
	updateSlots();
}

void MainWindow::undel()
{
	int selected_slot = cardslots->get_selected_slot();

	if (selected_slot==-1)
	{
		QMessageBox::information(this,"Error","Please select a block first.");
		return;
	}

	if (!psx_card->get_slot_is_free(selected_slot))
	{
		QMessageBox::information(this,"Error","This block is used.");
		return;
	}

	psx_card->undelete_slot(selected_slot);
	updateSlots();
}

void MainWindow::upload()
{
	int retry=0;

	if (!get_perm())
	{
		QMessageBox::information(this,"Error","Cannot obtain I/O permissions\n\n"
		"Please run this program with\n"
		"super user privileges.");
		return;
	}

    QProgressDialog qp(this);
    //"label 1","Stop",1024,this,"qp",true);

    qp.setRange(0,1024);
    qp.setValue(0);
    qp.setAutoClose(true);
	qp.setMinimumDuration(0);
    qp.setLabel(new QLabel("Writing to memory card..."));
	qp.show();
	for (int i=0; i<1024; i++)
	{
        qp.setValue( i );
		qApp->processEvents();

		//qp.setLabelText("Writing Block "+QString::number((i/64)+1,10)+"/16");
		if (retry==0) { qp.setLabelText("Writing Block "+QString::number((i/64)+1,10)+"/16"); }
		else
		{
			if (retry<1023)
			{
				qp.setLabelText("Retry #"+QString::number(retry));
			}
			else
			{
				qp.setLabelText("Still trying...(is there a memory card?)");
			}
		}
        if (qp.wasCanceled())
		{
			QMessageBox::warning(this,"Error","Data on memory card may be corrupted.");
			return;
		}

		//psx_card->save_card_frame(i);
		if (!psx_card->save_card_frame(i))
		{
			retry++;
			if (retry>=1024) { retry=1024; }
			i--;
		}
		else
		{
			retry=0;

		}
	}
	psx_card->update_data();
	updateSlots();
}

void MainWindow::download()
{
	int retry=0;

	if (!get_perm())
	{
		QMessageBox::information(this,"Error","Cannot obtain I/O permissions\n\n"
		"Please run this program with\n"
		"super user privileges.");
		return;
	}

    QProgressDialog qp("label 1","Stop",0,1024,this,Qt::Dialog);

    qp.setValue(0);
    qp.setAutoClose(true);
    qp.setMinimumDuration(0);
    QLabel *label = new QLabel("Reading from memory card...");
    qp.setLabel(label);
	qp.show();
	for (int i=0; i<1024; i++)
	{
        qp.setValue( i );
		qApp->processEvents();

		if (retry==0) { qp.setLabelText("Reading Block "+QString::number((i/64)+1,10)+"/16"); }
		else
		{
			if (retry<1023)
			{
                qp.setLabelText("Retry #"+QString::number(retry));
			}
			else
			{
				qp.setLabelText("Still trying...(is there a memory card?)");
			}
		}

        if (qp.wasCanceled()) { return; }

		if (psx_card->load_card_frame(i))
		{
			retry++;
			if (retry>=1024) { retry=1024; }
			i--;
		}
		else
		{
			retry=0;

		}
	}
	psx_card->update_data();
	updateSlots();
}

/**************************************************/

void MainWindow::dummy()
{
	QMessageBox::information(this,"Sorry","Not available yet...");
}

#define xstr(a) str(a)
#define str(a) #a

void MainWindow::showabout()
{
	QMessageBox::about(this,"About","PSX Memory Card Manager v" xstr(VERSION) "\n"
	"By Raphael Assenat\n"
	"Memory Card I/O by Richard Davies\n"
	"\nhttp://www.raphnet.net/programmation/psxmemcardmgr/index_en.php");
}

void MainWindow::showaboutQt()
{
	QMessageBox::aboutQt(this,"PSX Memory Card Manager v0.1");
}

void MainWindow::load(QString filename)
{
	psx_card->load_file(filename);  // Fill in the data holder
    updateSlots();
}

void MainWindow::save(QString filename)
{
	psx_card->save_file(filename);  // Fill in the data holder
}

void MainWindow::clear()
{
	psx_card->clearData();
	updateSlots();
}

void MainWindow::updateSlots()
{
	// Read data from the PSX_memory_card object
	// Write it into the cardslots widget
	for (int i=0; i<15; i++)
	{
		cardslots->changePcode(i,psx_card->get_slot_Pcode(i));
		cardslots->changeText(i,psx_card->get_slot_title(i));
		cardslots->changeIcon(i,psx_card->get_slot_icon(i));
		cardslots->changeGameID(i,psx_card->get_slot_gameID(i));
	}
}
