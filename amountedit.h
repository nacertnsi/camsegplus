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

#ifndef AMOUNTEDIT_H
#define AMOUNTEDIT_H

#include <QWidget>
#include <libCAMSEG/amount.h>
#include <libCAMSEG/quantity.h>

class QPushButton;
class QDoubleValidator;
class QLineEdit;
class QFrame;
class kMyMoneyCalculator;

class AmountEdit : public QWidget
{
    Q_OBJECT

    public:
                        AmountEdit(unsigned int p_nbDecimals = 2, QWidget* parent = NULL);
                        ~AmountEdit();

        double          minimum() const;
        double          maximum() const;
        int             precision() { return m_prec; };
        QString         prefix() const;
        QString         suffix() const;

        QString         cleanText() const;

        CAMSEG::Amount  amount() const;
        CAMSEG::Quantity quantity() const;
        double          doubleValue() const;
        int             intValue() const;

        void            setMinimum(const double p_amount);
        void            setMaximum(const double p_amount);
        void            setPrefix(const QString & p_prefix);
        void            setSuffix(const QString & p_suffix);

        void            setRange(const double p_min, const double p_max);

        QWidget*        focusWidget() const;
        void            setFocus();

        void            setCalculatorButtonVisible(const bool show);
        void            setAllowEmpty(bool allowed);

        bool            isCalculatorButtonVisible() const;
        bool            isEmptyAllowed() const;

        QLineEdit*      lineedit() const;

        bool            isReadOnly() const;
        void            setReadOnly(bool readOnly);

        void            setPrecision(const int prec);

        bool            isValid() const;

virtual bool            eventFilter(QObject * , QEvent * );

    protected:
        void            keyPressEvent(QKeyEvent* event);
        void            calculatorOpen(QKeyEvent* k);
        void            ensureFractionalPart();

    public slots:
        void            loadText(const QString& text);
        void            clearText();

        void            setText(const QString& txt) { setDoubleValue(txt.toDouble()); };
        void            setAmount(const CAMSEG::Amount & p_value);
        void            setQuantity(const CAMSEG::Quantity & p_value);
        void            setDoubleValue(const double p_value);
        void            setIntValue(const int p_value);

    protected slots:
        void            theTextChanged(const QString & text);
        void            slotCalculatorResult();
        void            slotCalculatorOpen();

    signals:
        void            valueChanged(double p_newValue);
        void            textChanged(const QString& text);

    private:
        bool            cursorIsBeforeDecimalSeparator() const;
        void            ensureFractionalPart(QString& txt) const;


        double              m_minValue;
        double              m_maxValue;

        QString             m_prefix;
        QString             m_suffix;

        QLineEdit*          m_lineEdit;
        QPushButton*        m_btnShowCalculator;
        kMyMoneyCalculator* m_calculator;
        QFrame*             m_calculatorFrame;

        QDoubleValidator*   m_validator;

        bool                m_allowEmpty;
        int                 m_prec;

        QString             previousText; // keep track of what has been typed
        QString             m_text;       // keep track of what was the original value
};

#endif // AMOUNTEDIT_H
