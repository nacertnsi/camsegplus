# -------------------------------------------------
# CAMSEG SCM
# Copyright (C) 2008-2010 CAMSEG Technologies
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------
QT += sql \
    network \
    webkit \
    xml \
    svg \
    script
!win32:TARGET = camsegscm.bin
win32:TARGET = camsegscm
win32:LIBS += "c:\Qt\4.8.0\bin\CAMSEG2.dll" \
    "c:\Qt\4.8.0\bin\CRL2.dll" \
    "c:\Qt\4.8.0\bin\qscintilla2.dll" \
    "c:\Qt\4.8.0\bin\wwwidgets4d.dll"
DESTDIR = bin
#RC_FILE = camseg_gui.rc
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui
TEMPLATE = app
CONFIG += wwwidgets
SOURCES += main.cpp \
    mainwindow.cpp \
    frmAbout.cpp \
    centralwidget.cpp \
    frmwelcome.cpp \
    frmmanagecustomers.cpp \
    frmmanagevendors.cpp \
    frmmanageproducts.cpp \
    frmmanageorders.cpp \
    frmmanagepayments.cpp \
    frmmanagedeliveries.cpp \
    frmmanagesuppliers.cpp \
    frmoverview.cpp \
    frmmanagetaxes.cpp \
    frmmanagecwi.cpp \
    camsegtable.cpp \
    global.cpp \
    frmidentification.cpp \
    camsegobjectinfos.cpp \
    camsegobjectmodifier.cpp \
    camsegdialog.cpp \
    frmlocationmodif.cpp \
    frmobjectmodif.cpp \
    frmdelivery.cpp \
    frmitemmodif.cpp \
    frmorder.cpp \
    frmmanagecat.cpp \
    frminventory.cpp \
    frmstockadjustments.cpp \
    frmadjustment.cpp \
    frmcashadjustments.cpp \
    frmreport.cpp \
    frmNouvelleCampagne.cpp \
    frmmanageusers.cpp \
    frmOptions.cpp \
    frmCampaignProperties.cpp \
    frmlookup.cpp \
    frmpay.cpp \
    frmmanagepay.cpp \
    frmstats.cpp \
    frmimportmodel.cpp \
    frmpicture.cpp \
    frmtraductor.cpp \
    frmcwiwizard.cpp \
    frmreportinstaller.cpp \
    frmplugins.cpp \
    plugin.cpp \
    frmthemecreator.cpp \
    frmregister.cpp \
    frmmanageplus.cpp \
    frmplu.cpp \
    frmdiscount.cpp \
    frmmanageunits.cpp \
    frmmanagediscounts.cpp \
    frmadddiscountcondition.cpp \
    frmadddiscountaction.cpp \
    frmcharts.cpp \
    frmreportviewer.cpp \
    frmchart.cpp \
    frmmanageservices.cpp \
    frmmanagedonations.cpp \
    frmmanagedonators.cpp \
    frmlogs.cpp \
    frmchangepermissions.cpp \
    frmselectdonator.cpp \
    frmproductlist.cpp \
    camsegproductlist.cpp \
    codeedit.cpp \
    frmaddviacopy.cpp \
    frmgeneratechart.cpp \
    frmdaterange.cpp \
    frmbatchprinting.cpp \
    frmunitconverter.cpp \
    frmlocaldb.cpp \
    frmopendb.cpp \
    dbinfoswidget.cpp \
    amountedit.cpp \
    autosavethread.cpp \
    posscreen.cpp \
    frmposbutton.cpp \
    posnumericpad.cpp \
    frmnumericpad.cpp \
    poscheckoutscreen.cpp \
    frmposlayoutconfig.cpp \
    posconfigwidget.cpp \
    frmpossettings.cpp \
    qtwin.cpp \
    frmbatchimport.cpp \
    kmymoneycalculator.cpp \
    table2d.cpp \
    frmcopycampaign.cpp \
    scmapplication.cpp \
    frmopennetwork.cpp \
    frmexportobjects.cpp \
    frmexecutequeries.cpp \
    frmmanageexpenses.cpp \
    frmexporttransactions.cpp \
    frmclosecurrentperiod.cpp \
    frmbrowsepreviousperiods.cpp \
    frmbatchprocessing.cpp \
    campaignmodel.cpp \
    wmstyle/windowsmodernstyle.cpp \
    dialogcheckproduct.cpp \
    simplecrypt/simplecrypt.cpp \
    dialog.cpp
HEADERS += mainwindow.h \
    frmAbout.h \
    global.h \
    centralwidget.h \
    frmwelcome.h \
    frmmanagecustomers.h \
    frmmanagevendors.h \
    frmmanageproducts.h \
    frmmanageorders.h \
    frmmanagepayments.h \
    frmmanagedeliveries.h \
    frmmanagesuppliers.h \
    frmoverview.h \
    frmmanagetaxes.h \
    frmmanagecwi.h \
    camsegtable.h \
    frmidentification.h \
    camsegobjectinfos.h \
    camsegobjectmodifier.h \
    camsegdialog.h \
    frmlocationmodif.h \
    frmobjectmodif.h \
    frmdelivery.h \
    frmitemmodif.h \
    frmorder.h \
    frmmanagecat.h \
    frminventory.h \
    frmstockadjustments.h \
    frmadjustment.h \
    frmcashadjustments.h \
    frmreport.h \
    frmNouvelleCampagne.h \
    frmmanageusers.h \
    frmOptions.h \
    frmCampaignProperties.h \
    frmlookup.h \
    frmpay.h \
    frmmanagepay.h \
    frmstats.h \
    frmimportmodel.h \
    frmpicture.h \
    frmtraductor.h \
    frmcwiwizard.h \
    frmreportinstaller.h \
    frmplugins.h \
    plugin.h \
    frmthemecreator.h \
    frmregister.h \
    frmmanageplus.h \
    frmplu.h \
    frmdiscount.h \
    frmmanageunits.h \
    frmmanagediscounts.h \
    frmadddiscountcondition.h \
    frmadddiscountaction.h \
    frmcharts.h \
    frmreportviewer.h \
    frmchart.h \
    frmmanageservices.h \
    frmmanagedonations.h \
    frmmanagedonators.h \
    frmlogs.h \
    frmchangepermissions.h \
    frmselectdonator.h \
    frmproductlist.h \
    camsegproductlist.h \
    codeedit.h \
    frmaddviacopy.h \
    frmgeneratechart.h \
    frmdaterange.h \
    frmbatchprinting.h \
    frmunitconverter.h \
    frmlocaldb.h \
    frmopendb.h \
    dbinfoswidget.h \
    amountedit.h \
    autosavethread.h \
    posscreen.h \
    frmposbutton.h \
    posnumericpad.h \
    frmnumericpad.h \
    poscheckoutscreen.h \
    frmposlayoutconfig.h \
    posconfigwidget.h \
    frmpossettings.h \
    qtwin.h \
    frmbatchimport.h \
    kmymoneycalculator.h \
    table2d.h \
    frmcopycampaign.h \
    scmapplication.h \
    frmopennetwork.h \
    frmexportobjects.h \
    frmexecutequeries.h \
    frmmanageexpenses.h \
    frmexporttransactions.h \
    frmclosecurrentperiod.h \
    frmbrowsepreviousperiods.h \
    frmbatchprocessing.h \
    campaignmodel.h \
    wmstyle/windowsmodernstyle.h \
    dialogcheckproduct.h \
    simplecrypt/simplecrypt.h \
    dialog.h \
    licenseTest.h
FORMS += mainwindow.ui \
    frmhelp.ui \
    centralwidget.ui \
    frmwelcome.ui \
    frmtraductor.ui \
    frmregister.ui \
    dialogcheckproduct.ui \
    dialog.ui
unix:LIBS += -lCAMSEG \
    -lCRL \
    -lbarcode \
    -Llib/
!win32:LIBS += -lziparch
#TRANSLATIONS += camseg_fr.ts \
 #   camseg_tr.ts


win32:INCLUDEPATH += "E:\release-2.6"
LIBS += -L"E:\Qt:\4.8.0\lib"
