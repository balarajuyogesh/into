/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#include "TestPiiSerialization.h"

#include <PiiSerializationUtil.h>
#include <PiiMatrixSerialization.h>
#include <PiiSerialization.h>
#include <PiiMatrixUtil.h>

struct Base
{
  enum ConstructionType { ConstructedBySerialization, ConstructedByUser };
  ConstructionType constructionType;

  Base(ConstructionType type) : constructionType(type) {}
  virtual ~Base() {}
  virtual int type() const { return 0; }

  PII_VIRTUAL_METAOBJECT_FUNCTION
  template <class Archive> void serialize(Archive&, const unsigned int) {}
};

#define PII_SERIALIZABLE_CLASS Base
#define PII_VIRTUAL_METAOBJECT
#define PII_SERIALIZABLE_IS_ABSTRACT
#include <PiiSerializableRegistration.h>

struct Derived : Base
{
  virtual int type() const { return 1; }

  Derived() : Base(ConstructedByUser) {}
  Derived(PiiSerialization::Void) : Base(ConstructedBySerialization) {}

  PII_VIRTUAL_METAOBJECT_FUNCTION
  template <class Archive> void serialize(Archive&, const unsigned int) {}
};

PII_SERIALIZATION_CONSTRUCTOR(Derived, Void, ());
PII_SERIALIZATION_NORMAL_CONSTRUCTOR(Derived, 0, ());

#define PII_SERIALIZABLE_CLASS Derived
#define PII_VIRTUAL_METAOBJECT
#define PII_CUSTOM_FACTORY
#include <PiiSerializableRegistration.h>

struct VariantTester
{
  VariantTester(int val=0) : iMember(val) {}

  int iMember;

  template <class Archive> void serialize(Archive& ar, const unsigned)
  {
    ar & iMember;
  }
};

Q_DECLARE_METATYPE(VariantTester);

#define PII_SERIALIZABLE_CLASS VariantTester
#define PII_USED_AS_QVARIANT
#include <PiiSerializableRegistration.h>

const int iVariantTesterMetaType = qRegisterMetaType<VariantTester>("VariantTester");

#include <PiiGenericTextInputArchive.h>
#include <PiiGenericTextOutputArchive.h>
#include <PiiGenericBinaryInputArchive.h>
#include <PiiGenericBinaryOutputArchive.h>
#include <PiiSharedPtr.h>

#include <iostream>

#include <QtTest>

// HACK QtTest defines "lst2" to be 1121 in Qt 5.0.2. How cool.
#ifdef lst2
#  undef lst2
#endif

class SharedObj : public PiiSharedObject
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& ar, const unsigned)
  {
    ar & iValue;
  }

public:
  SharedObj(int i = 0) : iValue(i) { ++iRefCount; }
  ~SharedObj() { --iRefCount; }
  int iValue;
  static int iRefCount;
};

int SharedObj::iRefCount = 0;

typedef PiiSharedPtr<SharedObj> SharedPtr;

TestPiiSerialization::TestPiiSerialization() : _dMat(3,3), _iMat(3,3), _cMat(3,3), _pDMat(0)
{
  //Adjust tests objects to correct state
  _pDMat = new PiiMatrix<double>(20,1);
  for ( int i=0; i<_pDMat->rows(); i++ )
    (*_pDMat)(i,0) = i;

  _dMat = 1.0;
  _iMat = 2;
  _cMat = 'k';
}


void TestPiiSerialization::derivedTypes()
{
  //qDebug() << PiiSerializationFactory::keys();
  Base* b = PiiSerializationFactory::create<Base>("Derived");
  QVERIFY(b != 0);
  QCOMPARE(b->type(), 1);
  QCOMPARE(b->constructionType, Base::ConstructedByUser);

  QByteArray array;
  QBuffer buffer(&array);
  buffer.open(QIODevice::ReadWrite);

  try
    {
      {
        PiiGenericTextOutputArchive oa(&buffer);
        oa << b;
        delete b;
        b = 0;
      }
      //qDebug() << array;
      {
        buffer.seek(0);
        PiiGenericTextInputArchive ia(&buffer);
        ia >> b;
        QVERIFY(b != 0);
        QCOMPARE(b->type(), 1);
        QCOMPARE(b->constructionType, Base::ConstructedBySerialization);
      }
    }
  catch (PiiException& ex)
    {
      QFAIL(qPrintable(ex.message()));
    }

}

template <class InputArchive, class OutputArchive>
void TestPiiSerialization::anyArchive()
{
  QByteArray array;
  QBuffer buffer(&array);
  QDateTime currentDateTime(QDateTime::currentDateTime());
  currentDateTime.setTimeSpec(Qt::UTC);
  try
    {
      QString sourceStr("01230Äö Å\xfe");
      sourceStr[4] = 0;
      const char* sourcePtr = "Ååå, hur härlig!";
      const char* sourcePtr2 = sourcePtr;
      QString* sourceStrPtr = &sourceStr;
      PiiMatrix<double> matrix(5, 5);
      Pii::generate(matrix.begin(), matrix.end(), Pii::CountFunction<double>());

      QList<SharedPtr> lst;
      SharedPtr ptr(new SharedObj(123));
      lst << ptr;
      lst << ptr;
      QCOMPARE(SharedObj::iRefCount, 1);

      QVariantList lst2;
      lst2 << QVariant::fromValue(VariantTester(0));
      lst2 << QVariant::fromValue(VariantTester(1));

      {
        buffer.open(QIODevice::ReadWrite);
        OutputArchive oa(&buffer);

        oa << sourceStr;
        oa << sourcePtr;
        oa << 1;
        oa << sourcePtr2;
        oa << sourceStrPtr;
        oa << matrix;
        oa << currentDateTime;
        oa << lst;
        oa << lst2;
      }

      /*for (int i=0; i<array.size(); ++i)
        std::cout << array[i];
      std::cout << "\n";
      */
      //qDebug() << array;
      //qDebug() << array.size();

      {
        buffer.seek(0);
        InputArchive ia(&buffer);

        QString resultStr;
        ia >> resultStr;
        QCOMPARE(resultStr, sourceStr);
        QCOMPARE(QChar(resultStr[4]), QChar(0));

        char* resultPtr;
        ia >> resultPtr;
        QCOMPARE(std::strcmp(sourcePtr, resultPtr), 0);

        int number;
        ia >> number;
        QCOMPARE(number,1);

        char* resultPtr2;
        ia >> resultPtr2;
        QCOMPARE(resultPtr, resultPtr2);

        QString* resultStrPtr;
        ia >> resultStrPtr;
        QCOMPARE(*resultStrPtr, resultStr);

        PiiMatrix<double> matrix2;
        ia >> matrix2;
        //QDebug d = qDebug();
        //Pii::matlabPrint(d, matrix2);
        QVERIFY(Pii::equals(matrix, matrix2));

        delete resultStrPtr;
        delete[] resultPtr;

        QDateTime newDateTime;
        ia >> newDateTime;
        QCOMPARE(currentDateTime, newDateTime);

        QList<SharedPtr> lst;
        ia >> lst;
        QCOMPARE(lst.size(), 2);
        QCOMPARE(lst[0]->iValue, 123);
        QCOMPARE(lst[1]->iValue, 123);
        QCOMPARE(SharedObj::iRefCount, 2);

        QVariantList lst2;
        ia >> lst2;
        QCOMPARE(lst2.size(), 2);
        QCOMPARE(lst2[0].value<VariantTester>().iMember, 0);
        QCOMPARE(lst2[1].value<VariantTester>().iMember, 1);
      }
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + " at " +
             ex.location() + ". Additional info: " + ex.info()).toLocal8Bit().constData());
    }
  QCOMPARE(SharedObj::iRefCount, 0);
}

void TestPiiSerialization::textArchive()
{
  anyArchive<PiiGenericTextInputArchive,PiiGenericTextOutputArchive>();
}

void TestPiiSerialization::binaryArchive()
{
  anyArchive<PiiGenericBinaryInputArchive,PiiGenericBinaryOutputArchive>();
}

QTEST_MAIN(TestPiiSerialization)

