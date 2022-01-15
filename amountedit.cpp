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

#include "amountedit.h"

#include <QKeyEvent>
#include <QDoubleValidator>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLocale>
#include <QHBoxLayout>
#include <QDebug>

#include "global.h"
#include "mainwindow.h"
#include "kmymoneycalculator.h"

#include <libCAMSEG/currency.h>

using namespace CAMSEG;

//AmountEdit

AmountEdit::AmountEdit(unsigned int p_nbDecimals, QWidget* parent) : QWidget(parent), m_prec(p_nbDecimals)
{
    m_lineEdit = new QLineEdit();

    m_calculatorFrame = new QFrame(this);
    m_calculatorFrame->setWindowFlags(Qt::Popup);
    m_calculatorFrame->setFrameStyle(QFrame::Panel);
    m_calculatorFrame->setLineWidth(3);

    m_calculator = new kMyMoneyCalculator(m_calculatorFrame);
    m_calculator->setVisible(false);
    m_calculatorFrame->setFixedSize(m_calculator->width()+3, m_calculator->height()+3);
    m_calculatorFrame->hide();

    m_btnShowCalculator = new QPushButton(QIcon(PATH_ICONS + "calc.png"), "");
    m_btnShowCalculator->setFocusProxy(m_lineEdit);

    m_validator = new QDoubleValidator(this);
    m_lineEdit->setValidator(m_validator);

    m_validator->setDecimals(p_nbDecimals);
    m_validator->setNotation(QDoubleValidator::StandardNotation);

    m_allowEmpty = false;

    setRange(0, 1000000000);

    m_lineEdit->setAlignment(Qt::AlignRight);
    m_lineEdit->installEventFilter(this);

    m_btnShowCalculator->setFixedWidth(m_btnShowCalculator->sizeHint().height());
    m_btnShowCalculator->setFixedHeight(m_lineEdit->sizeHint().height());

    QString deftext = "0.";

    for (int i = 0; i < p_nbDecimals; ++i)
        deftext += "0";

    if (p_nbDecimals == 0)
        deftext = "0";

    m_text = deftext;
    m_lineEdit->setText(deftext);

    //Layout
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_lineEdit);
    mainLayout->addWidget(m_btnShowCalculator);

    //Connexions
    connect(m_lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(theTextChanged(const QString&)));
    connect(m_btnShowCalculator, SIGNAL(clicked()), this, SLOT(slotCalculatorOpen()));
    connect(m_calculator, SIGNAL(signalResultAvailable()), this, SLOT(slotCalculatorResult()));
    connect(m_calculator, SIGNAL(signalPleaseClose()), m_calculatorFrame, SLOT(hide()));

}

AmountEdit::~AmountEdit()
{
    delete m_lineEdit;
    delete m_calculatorFrame;
    delete m_validator;
    delete m_btnShowCalculator;
}

void AmountEdit::slotCalculatorResult()
{
    QString result;

    if(m_calculator != 0)
    {
        m_calculator->setVisible(false);
        m_calculatorFrame->hide();
        m_lineEdit->setText(QString::number(m_calculator->result().toDouble(), 'f', m_prec));
        ensureFractionalPart();
        emit valueChanged(m_lineEdit->text().toDouble());
        m_text = m_lineEdit->text();
    }
}

void AmountEdit::slotCalculatorOpen()
{
  calculatorOpen(NULL);
}

void AmountEdit::calculatorOpen(QKeyEvent* k)
{
  m_calculator->setInitialValues(m_lineEdit->text(), k);

  int h = m_calculatorFrame->height();
  int w = m_calculatorFrame->width();

  // usually, the calculator widget is shown underneath the MoneyEdit widget
  // if it does not fit on the screen, we show it above this widget
  QPoint p = mapToGlobal(QPoint(0,0));
  if(p.y() + height() + h > QApplication::desktop()->height())
    p.setY(p.y() - h);
  else
    p.setY(p.y() + height());

  // usually, it is shown left aligned. If it does not fit, we align it
  // to the right edge of the widget
  if(p.x() + w > QApplication::desktop()->width())
    p.setX(p.x() + width() - w);

  QRect r = m_calculator->geometry();
  r.moveTopLeft(p);
  m_calculatorFrame->setGeometry(r);
  m_calculatorFrame->show();
  m_calculator->setVisible(true);
  m_calculator->setFocus();
}

double AmountEdit::minimum() const
{
    return m_minValue;
}

double AmountEdit::maximum() const
{
    return m_maxValue;
}

QString AmountEdit::prefix() const
{
    return m_prefix;
}

QString AmountEdit::suffix() const
{
    return m_suffix;
}

QString AmountEdit::cleanText() const
{
    return m_lineEdit->text();
}

Amount AmountEdit::amount() const
{
    return Amount(m_lineEdit->text());
}

Quantity AmountEdit::quantity() const
{
    return Quantity(m_lineEdit->text());
}

double AmountEdit::doubleValue() const
{
    return m_lineEdit->text().toDouble();
}

int AmountEdit::intValue() const
{
    return Amount(m_lineEdit->text()).roundToInt();
}

void AmountEdit::setMinimum(const double p_amount)
{
    m_minValue = p_amount;
    m_validator->setBottom(p_amount);
}

void AmountEdit::setMaximum(const double p_amount)
{
    m_maxValue = p_amount;
    m_validator->setTop(p_amount);
}

void AmountEdit::setPrefix(const QString & p_prefix)
{
    m_prefix = p_prefix;
}

void AmountEdit::setSuffix(const QString & p_suffix)
{
    m_suffix = p_suffix;
}

void AmountEdit::setRange(const double p_min, const double p_max)
{
    setMinimum(p_min);
    setMaximum(p_max);
}

void AmountEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F12)
    {
        slotCalculatorOpen();
    }
    else
    {
        event->ignore();
    }
}

void AmountEdit::theTextChanged(const QString & theText)
{
    QLocale l;
    QString d = l.decimalPoint();
    QString l_text = theText;
    QString nsign, psign;

    nsign = l.negativeSign();
    psign = l.positiveSign();

    int i = 0;

    if (isEnabled())
    {
        QValidator::State state =  m_lineEdit->validator()->validate( l_text, i);

        if (state == QValidator::Intermediate)
        {
            if (l_text.length() == 1)
            {
                if(l_text != d && l_text != nsign && l_text != psign)
                    state = QValidator::Invalid;
            }
        }
        if (state==QValidator::Invalid)
        {
            m_lineEdit->setText(previousText);
        }
        else
        {
            previousText = l_text;
            emit textChanged(m_lineEdit->text());
        }
    }
}

void AmountEdit::setAmount(const Amount & p_value)
{
    loadText(p_value.toString());
}

void AmountEdit::setQuantity(const Quantity & p_value)
{
    loadText(QString::number(p_value.toDouble(), 'f', m_prec));
}

void AmountEdit::setDoubleValue(const double p_value)
{
    loadText(QString::number(p_value, 'f', m_prec));
}

void AmountEdit::setIntValue(const int p_value)
{
    loadText(QString::number(p_value));
}

void AmountEdit::loadText(const QString& txt)
{
  m_lineEdit->setText(txt);

  ensureFractionalPart();

  m_text = m_lineEdit->text();
}

void AmountEdit::clearText(void)
{
  m_text.clear();
  m_lineEdit->setText(m_text);
}

QWidget* AmountEdit::focusWidget() const
{
  QWidget* w = m_lineEdit;
  while(w->focusProxy())
    w = w->focusProxy();
  return w;
}

void AmountEdit::setFocus()
{
    m_lineEdit->selectAll();
    m_lineEdit->setFocus();
}

void AmountEdit::setCalculatorButtonVisible(const bool show)
{
    m_btnShowCalculator->setVisible(show);
}

void AmountEdit::setAllowEmpty(bool allowed)
{
    m_allowEmpty = allowed;
}

bool AmountEdit::isCalculatorButtonVisible() const
{
    return m_btnShowCalculator->isVisible();
}

bool AmountEdit::isEmptyAllowed() const
{
    return m_allowEmpty;
}

QLineEdit* AmountEdit::lineedit() const
{
    return m_lineEdit;
}

bool AmountEdit::isReadOnly() const
{
  if(m_lineEdit)
    return m_lineEdit->isReadOnly();
  return false;
}

void AmountEdit::setReadOnly(bool readOnly)
{
  if(m_lineEdit)
    m_lineEdit->setReadOnly(readOnly);
}

void AmountEdit::setPrecision(const int prec)
{
    if (prec >= -1 && prec <= 20)
    {
        if (prec != m_prec)
        {
            m_validator->setDecimals(prec);
            m_prec = prec;
            // update current display
            setDoubleValue(doubleValue());
        }
    }
}

bool AmountEdit::isValid() const
{
    return !(m_lineEdit->text().isEmpty());
}

bool AmountEdit::eventFilter(QObject * , QEvent * e)
{
    bool rc = false;

  // we want to catch some keys that are usually handled by
  // the base class (e.g. '+', '-', etc.)
  if(e->type() == QEvent::KeyPress) {
    QKeyEvent *k = static_cast<QKeyEvent *> (e);

    rc = true;
    switch(k->key()) {
      case Qt::Key_Plus:
      case Qt::Key_Minus:
        if(m_lineEdit->hasSelectedText()) {
          m_lineEdit->cut();
        }
        if(m_lineEdit->text().length() == 0) {
          rc = false;
          break;
        }
        // in case of '-' we do not enter the calculator when
        // the current position is the beginning and there is
        // no '-' sign at the first position.
        if(k->key() == Qt::Key_Minus) {
          if(m_lineEdit->cursorPosition() == 0 && m_lineEdit->text()[0] != '-') {
            rc = false;
            break;
          }
        }
        // otherwise, tricky fall through here!

      case Qt::Key_Slash:
      case Qt::Key_Asterisk:
      case Qt::Key_Percent:
        if(m_lineEdit->hasSelectedText()) {
          // remove the selected text
          m_lineEdit->cut();
        }
        calculatorOpen(k);
        break;

      default:
        rc = false;
        break;
    }

  } else if(e->type() == QEvent::FocusOut) {
    if(!m_lineEdit->text().isEmpty() || !m_allowEmpty)
      ensureFractionalPart();

    if(Amount(m_lineEdit->text().toDouble()) != Amount(m_text.toDouble())
    && !m_calculator->isVisible()) {
      emit valueChanged(m_lineEdit->text().toDouble());
    }
    m_text = m_lineEdit->text();
  }
  e->accept();
  return rc;
}

bool AmountEdit::cursorIsBeforeDecimalSeparator() const
{
    bool returnValue = false;

    //Find the current decimal separator position
    int position = m_lineEdit->text().indexOf('.');

    if (position >= 0)
    {
        returnValue = m_lineEdit->cursorPosition() + 1 < position;
    }

    return returnValue;
}

void AmountEdit::ensureFractionalPart()
{
    QString s(m_lineEdit->text());
    ensureFractionalPart(s);
    m_lineEdit->setText(s);
}

void AmountEdit::ensureFractionalPart(QString& s) const
{
    QLocale locale;
    QString decimalSymbol = locale.decimalPoint();

    if (decimalSymbol.isEmpty())
        decimalSymbol = '.';

    // If text contains no 'monetaryDecimalSymbol' then add it
    // followed by the required number of 0s
    if (!s.isEmpty())
    {
        if (m_prec > 0)
        {
            if (!s.contains(decimalSymbol))
            {
                s += decimalSymbol;

                for (int i=0; i < m_prec; i++)
                    s += '0';
            }
            else
            {
                //Check the required amount of decimals
                int decimals = s.count() - s.indexOf(decimalSymbol) - 1;

                for (int i = decimals; i < m_prec; ++i)
                    s += '0';

            }
        }
        else if (m_prec == 0)
        {
            while (s.contains(decimalSymbol))
            {
                int pos = s.lastIndexOf(decimalSymbol);

                if(pos != -1)
                {
                    s.truncate(pos);
                }
            }
        }
        else if (s.contains(decimalSymbol))
        {  // m_prec == -1 && fraction

            // no trailing zeroes
            while(s.endsWith('0'))
            {
                s.truncate(s.length()-1);
            }

            // no trailing decimalSymbol
            if(s.endsWith(decimalSymbol))
                s.truncate(s.length()-1);
        }
    }
}
