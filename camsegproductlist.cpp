/*
CAMSEG SCM
Copyright (C) 2008-2010 CAMSEG Technologies

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define MARGIN_SIZE 10
#define ITEM_HEIGHT 85

#include "camsegproductlist.h"
#include "global.h"

#include <QFile>
#include <QPainter>
#include <libCAMSEG/plu.h>

using namespace CAMSEG;

CAMSEGProductList::CAMSEGProductList(Campaign* p_campaign, QWidget* parent) :
        QWidget(parent), m_campaign(p_campaign)
{
    //Fill the row list
    for (unsigned int i = 0; i < m_campaign->getNumberOfPLUs(); i++)
    {
        PLU* currentPLU = m_campaign->getPLU(i);

        if (currentPLU->isInCatalogue())
        {
            CatalogueRow row;
            row.code = currentPLU->getFormattedId();
            row.image = currentPLU->getPicture().toImage();
            row.title = currentPLU->getName();
            row.description = currentPLU->getInformations();
            row.price = m_campaign->formatCurrency(currentPLU->getPriceSold());

            listRows << row;
        }
    }

    m_campaignName = m_campaign->getCampaignName();
    m_type = Catalogue_WithPictures;
    m_showGrid = false;

    unsigned int itemHeight = ITEM_HEIGHT;

    if (displayType() == Catalogue_SimpleList)
        itemHeight = Chart::getPixelsForPointSize(10, this);

    verticalSize = (listRows.count() * (itemHeight + MARGIN_SIZE)) + (MARGIN_SIZE * 2) + 100;
    setFixedHeight(verticalSize);
}

QSize CAMSEGProductList::minimumSizeHint()
{
    return QSize(300, verticalSize);
}

QSize CAMSEGProductList::sizeHint()
{
    return QSize(500, verticalSize);
}

bool CAMSEGProductList::saveAsPDF(const QString & p_path)
{
    QPrinter printer;
    QString path = p_path;

    if ( ! path.endsWith(".pdf"))
        path += ".pdf";

    QFile file(path);

    if (file.open(QIODevice::WriteOnly))
    {
        file.close();
    }
    else
    {
        return false;
    }

    printer.setOutputFileName(path);

    paintOnDevice(&printer, Device_Printer);

    return true;
}

void CAMSEGProductList::print(QPrinter* p_printer)
{
    paintOnDevice(p_printer, Device_Printer);
}

CAMSEGProductList::DisplayType CAMSEGProductList::displayType() const
{
    return m_type;
}

bool CAMSEGProductList::isGridVisible() const
{
    return m_showGrid;
}

void CAMSEGProductList::setDisplayType(const DisplayType p_type)
{
    m_type = p_type;
    update();
}

void CAMSEGProductList::setGridVisible(const bool p_value)
{
    m_showGrid = p_value;
    update();
}

void CAMSEGProductList::paintEvent(QPaintEvent*)
{
    paintOnDevice(this);
}

void CAMSEGProductList::paintOnDevice(QPaintDevice * p_device, DeviceType p_type)
{
    //Variable Initialisation
    QRect deviceRect;
    QRect internalRect;
    unsigned int deviceWidth;
    unsigned int deviceHeight;
    unsigned vPagePosition = 0;
    QPrinter* printer;
    QRect pluRect;
    QFont currentFont;

    //Constants values
    const unsigned int TITLE_POINTSIZE = 18;
    const unsigned int SUBTITLE_POINTSIZE = 14;

    switch (p_type)
    {
        case Device_Widget:
            deviceRect = rect();
            deviceWidth = width();
            deviceHeight = height();
            break;
        case Device_Printer:
            printer = dynamic_cast<QPrinter*>(p_device);
            deviceRect = printer->pageRect();
            deviceWidth = deviceRect.width();
            deviceHeight = deviceRect.height();
            break;
        case Device_File:
            deviceRect = QRect(0, 0, p_device->width(), p_device->height());
            deviceWidth = p_device->width();
            deviceHeight = p_device->height();
            break;
    }


    //---------------------Painter Creation---------------------
    QPainter painter(p_device);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    //painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::SolidLine);
    painter.setPen(Qt::black);

    //-------------Initializations & Other Constants------------
    internalRect = QRect(MARGIN_SIZE, MARGIN_SIZE, deviceWidth - (2 * MARGIN_SIZE), deviceHeight - (2 * MARGIN_SIZE));
    pluRect = QRect(internalRect.left(), internalRect.top(), internalRect.width(), ITEM_HEIGHT);

    if (displayType() == Catalogue_SimpleList)
        pluRect.setHeight(Chart::getPixelsForPointSize(10, p_device));

    const unsigned int WIDTH_PICTURE = ITEM_HEIGHT;
    const unsigned int WIDTH_DESCRIPTION = 0.65 * (double) pluRect.width();
    const unsigned int WIDTH_PRICE = pluRect.width() - (WIDTH_PICTURE + WIDTH_DESCRIPTION);

    //---------------------Background fill----------------------
    painter.fillRect(deviceRect, Qt::white);

    //---------------------Title & Subtitle---------------------
    unsigned int distTop = 0;

    //Title
    QRect titleRect = QRect(internalRect.topLeft(), QSize(internalRect.width(), Chart::getPixelsForPointSize(TITLE_POINTSIZE, p_device) + 2));
    currentFont.setPointSize(TITLE_POINTSIZE);
    currentFont.setBold(true);
    painter.setFont(currentFont);

    painter.drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeft, tr("Product Catalogue"));

    distTop += titleRect.height();

    //Subtitle
    QRect subtitleRect = QRect(QPoint(internalRect.left(), internalRect.top() + distTop), QSize(internalRect.width(), Chart::getPixelsForPointSize(SUBTITLE_POINTSIZE, p_device) + 2));
    currentFont.setPointSize(SUBTITLE_POINTSIZE);
    currentFont.setBold(false);
    painter.setFont(currentFont);

    painter.drawText(subtitleRect, Qt::AlignVCenter | Qt::AlignLeft, m_campaignName);

    distTop += subtitleRect.height();
    pluRect.translate(0, distTop + 30);

    //-----------------------Main Content-----------------------
    for (int i = 0; i < listRows.count(); i++)
    {
        CatalogueRow row = listRows[i];

        //------------Paint the PLU stuff------------

        //Grid
        if (m_showGrid)
        {
            QRect gridRect(pluRect);
            gridRect.setHeight(pluRect.height() + MARGIN_SIZE);

            painter.drawRect(gridRect);
            painter.drawLine(gridRect.left() + WIDTH_PICTURE + 1, gridRect.top(), gridRect.left() + WIDTH_PICTURE + 1, gridRect.bottom());
            painter.drawLine(pluRect.right() - WIDTH_PRICE, gridRect.top(), pluRect.right() - WIDTH_PRICE, gridRect.bottom());
        }

        //Picture
        QRect pictureRect(pluRect.left() + 1, pluRect.top() + 1, WIDTH_PICTURE - 2, pluRect.height() - 2);

        if (displayType() == Catalogue_WithPictures)
        {
            painter.drawImage(pictureRect, row.image.scaled(pictureRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            currentFont.setPointSize(9);
            currentFont.setBold(true);
            painter.setFont(currentFont);

            if (displayType() != Catalogue_SimpleList)
            {
                painter.drawText(pictureRect, Qt::AlignCenter, row.code);
            }
            else
            {
                pictureRect.translate(2, 1);
                painter.drawText(pictureRect, Qt::AlignTop | Qt::AlignLeft, row.code);
            }
        }

        //Title
        QRect titleRect(pluRect.left() + WIDTH_PICTURE + 3, pluRect.top() + 2, WIDTH_DESCRIPTION - 6, ITEM_HEIGHT);

        currentFont.setPointSize(11);

        if (displayType() == Catalogue_SimpleList)
            currentFont.setPointSize(9);

        currentFont.setBold(true);
        painter.setFont(currentFont);

        painter.drawText(titleRect, Qt::AlignTop | Qt::AlignLeft, row.title);

        //Description
        if (displayType() != Catalogue_SimpleList)
        {
            QRect descriptionRect(pluRect.left() + WIDTH_PICTURE + 3, pluRect.top() + 5 + Chart::getPixelsForPointSize(16, p_device), WIDTH_DESCRIPTION - 6, ITEM_HEIGHT);
            currentFont.setPointSize(9);
            currentFont.setBold(false);
            painter.setFont(currentFont);

            painter.drawText(descriptionRect, Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap, row.description);
        }

        //Price
        QRect priceRect(pluRect.right() - WIDTH_PRICE, pluRect.top(), WIDTH_PRICE, pluRect.height());

        currentFont.setPointSize(12);

        if (displayType() == Catalogue_SimpleList)
            currentFont.setPointSize(9);

        currentFont.setBold(false);
        painter.setFont(currentFont);

        if (displayType() != Catalogue_SimpleList)
        {
            painter.drawText(priceRect, Qt::AlignCenter, row.price);
        }
        else
        {
            priceRect.translate(0, 2);
            painter.drawText(priceRect, Qt::AlignTop | Qt::AlignHCenter, row.price);
        }



        //-------------Translate the rect------------
        pluRect.translate(0, pluRect.height() + MARGIN_SIZE);
        vPagePosition += pluRect.height() + MARGIN_SIZE;

        //If Printer
        if (p_type == Device_Printer)
        {
            if (vPagePosition + (pluRect.height() + MARGIN_SIZE) > deviceHeight && i < listRows.count() - 1) //Next too large
            {
                printer->newPage();
                vPagePosition = 0;
            }
        }
    }


}


