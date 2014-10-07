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

#include <PiiYdinTypes.h>
#include <QtDebug>
#include "TestPiiVariant.h"
#include <PiiVariant.h>
#include <PiiMatrix.h>

#include <PiiGenericTextOutputArchive.h>
#include <PiiGenericTextInputArchive.h>
typedef PiiGenericTextOutputArchive OutputArchive;
typedef PiiGenericTextInputArchive InputArchive;

#include <QtTest>

struct BigType
{
  template <class Archive> void serialize(Archive&, const unsigned int) {}

  BigType() { ++iCount; }
  BigType(const BigType& other) { ++iCount; std::memcpy(bigBuffer, other.bigBuffer, sizeof(bigBuffer)); }
  ~BigType() { --iCount; }
  bool operator== (const BigType&) const { return true; }
  int bigBuffer[16];
  static int iCount;
};

PII_DECLARE_VARIANT_TYPE(BigType, 0x666);
PII_REGISTER_VARIANT_TYPE(BigType);

PII_MAP_VARIANT_ID_TO_TYPE(0x667, BigType);

int BigType::iCount = 0;

void TestPiiVariant::serialization()
{
  QByteArray array;
  QBuffer buffer(&array);
  //qDebug() << PiiSerializer<PiiGenericOutputArchive>::map();
  try
    {
      PiiVariant
        obj0,
        obj1(1),
        obj2(true),
        obj3(0.7),
        obj4(QString("Test string.")),
        obj5(PiiMatrix<int>(2,2,
                            1,2,
                            3,4));

      buffer.open(QIODevice::ReadWrite);
      OutputArchive oa(&buffer);
      oa << obj0;
      oa << obj1;
      oa << obj2;
      oa << obj3;
      oa << obj4;
      oa << obj5;
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + ". Additional info: " + ex.info()).toUtf8().constData());
    }

  //qDebug() << array;

  try
    {
      buffer.seek(0);
      InputArchive ia(&buffer);
      PiiVariant obj;
      ia >> obj;
      QCOMPARE(obj.type(), (unsigned)PiiVariant::InvalidType);
#define READ_CHECK(TYPE,VAL)                     \
      ia >> obj;                                 \
      QCOMPARE(obj.type(), Pii::typeId<TYPE>()); \
      QCOMPARE(obj.valueAs<TYPE>(), VAL)

      READ_CHECK(int, 1);
      READ_CHECK(bool, true);
      READ_CHECK(double, 0.7);
      READ_CHECK(QString, QString("Test string."));

      ia >> obj;
      QCOMPARE(obj.type(), (unsigned)PiiYdin::IntMatrixType);
      QVERIFY(Pii::equals(obj.valueAs<PiiMatrix<int> >(),
                          PiiMatrix<int>(2,2,
                                         1,2,
                                         3,4)));
    }
  catch (PiiSerializationException& ex)
    {
      QFAIL(("Serialization error: " + ex.message() + ". Additional info: " + ex.info()).toUtf8().constData());
    }
}

void TestPiiVariant::construct()
{
  PiiVariant invalid;
  QCOMPARE(invalid.type(), (unsigned)PiiVariant::InvalidType);
  QVERIFY(!invalid.isValid());

#define TEST_TYPE(TYPE,VALUE)                   \
  {                                             \
    PiiVariant v1((TYPE)VALUE);                 \
    PiiVariant v2(v1);                          \
    QCOMPARE(v1.type(), Pii::typeId<TYPE>());   \
    QCOMPARE(v1.valueAs<TYPE>(), (TYPE)VALUE);  \
    QCOMPARE(v2.type(), Pii::typeId<TYPE>());   \
    QCOMPARE(v2.valueAs<TYPE>(), (TYPE)VALUE);  \
    QVERIFY(v1.isPrimitive());                  \
  }

  TEST_TYPE(char, 'a');
  TEST_TYPE(short, 1);
  TEST_TYPE(int, -1);
  TEST_TYPE(qint64, 0);

  TEST_TYPE(unsigned char, 'a');
  TEST_TYPE(unsigned short, 1);
  TEST_TYPE(unsigned int, -1);
  TEST_TYPE(quint64, 0);

  TEST_TYPE(float, 1.8f);
  TEST_TYPE(double, -1e-4);

  TEST_TYPE(bool, false);
}

void TestPiiVariant::copy()
{
  BigType obj;
  for (unsigned int i=0; i<16; ++i)
    obj.bigBuffer[i] = i;
  PiiVariant v1(obj);
  PiiVariant v2(v1);
  QCOMPARE(BigType::iCount, 3);
  PiiVariant v3(0);
  PiiVariant v4;
  PiiVariant v5(PiiMatrix<double>(0,0));

#define TEST_BUFFER(VAR)                        \
  QCOMPARE(VAR.type(), Pii::typeId<BigType>()); \
  for (int i=0; i<16; ++i)                            \
    QCOMPARE(VAR.valueAs<BigType>().bigBuffer[i], i)

  TEST_BUFFER(v1);
  TEST_BUFFER(v2);
  v3 = v2;
  TEST_BUFFER(v3);
  QCOMPARE(BigType::iCount, 4);

  v3 = v5;
  QVERIFY(v3.type() == (unsigned)PiiYdin::DoubleMatrixType);
  QVERIFY(v3.valueAs<PiiMatrix<double> >().isEmpty());

  v1 = v2 = v3 = v4;
  QCOMPARE(BigType::iCount, 1);
  QVERIFY(!v1.isValid());
  QVERIFY(!v2.isValid());
  QVERIFY(!v3.isValid());
  QVERIFY(!v4.isValid());
}

void TestPiiVariant::mapType()
{
  QCOMPARE(BigType::iCount, 0);
  {
    PiiVariant v(BigType(), 0x667);
    QCOMPARE(BigType::iCount, 1);
  }
  QCOMPARE(BigType::iCount, 0);
}

void TestPiiVariant::canConvert()
{
  QVERIFY(PiiVariant::canConvert(PiiVariant::IntType, PiiVariant::DoubleType));
  QVERIFY(PiiVariant::canConvert(PiiVariant::DoubleType, PiiVariant::IntType));
  QVERIFY(PiiVariant::canConvert(PiiVariant::BoolType, PiiVariant::FloatType));
  QVERIFY(PiiVariant::canConvert(PiiVariant::UnsignedCharType, PiiVariant::ShortType));
  QVERIFY(PiiVariant::canConvert(PiiVariant::DoubleType, PiiVariant::UnsignedIntType));
  QVERIFY(!PiiVariant::canConvert(PiiVariant::DoubleType, PiiVariant::CharType));
  QVERIFY(!PiiVariant::canConvert(PiiVariant::UnsignedCharType, PiiVariant::FloatType));
  QVERIFY(!PiiVariant::canConvert(PiiVariant::VoidPtrType, PiiVariant::DoubleType));
  PiiVariant i(1);
  QVERIFY(i.canConvert(PiiVariant::ShortType));
}

void TestPiiVariant::convertTo()
{
  PiiVariant i(123);
  QCOMPARE(i.convertTo<int>(), 123);
  QCOMPARE(i.convertTo<double>(), 123.0);
  QCOMPARE(i.convertTo<ushort>(), ushort(123));
  QCOMPARE(i.convertTo<bool>(), true);

  PiiVariant b(true);
  QCOMPARE(b.convertTo<int>(), 1);
  QCOMPARE(b.convertTo<double>(), 1.0);

  PiiVariant c('a');
  bool bOk = true;
  QCOMPARE(c.type(), uint(PiiVariant::CharType));
  QCOMPARE(c.convertTo<double>(&bOk), 0.0);
  QVERIFY(!bOk);
}

void TestPiiVariant::typeName()
{
  BigType t;
  PiiVariant
    v1(1),
    v2((void*)&v1),
    v3(PiiMatrix<uchar>(0,0)),
    v4(t);
  QCOMPARE(v1.typeName(), "int");
  QCOMPARE(v2.typeName(), "void*");
  QCOMPARE(v3.typeName(), "PiiMatrix<uchar>");
  QCOMPARE(v4.typeName(), "BigType");
}

void TestPiiVariant::equals()
{
  PiiVariant
    v1(123),
    v2(123),
    v3(QString("abc")),
    v4(PiiMatrix<int>(1, 2, 1, 2)),
    v5(PiiMatrix<int>(1, 2, 1, 2)),
    v6(PiiMatrix<int>(1, 2, 2, 2));

  QVERIFY(v1 == v1);
  QVERIFY(v1 == v2);
  QVERIFY(v1 != v3);
  QVERIFY(v3 != v4);
  QVERIFY(v4 == v4);
  QVERIFY(v4 == v5);
  QVERIFY(v4 != v6);
}

void TestPiiVariant::toQVariant()
{
  PiiVariant
    v1(123),
    v2(QString("abc")),
    v3(PiiMatrix<int>(1, 2, 2, 2));

  QVariant qv1(v1.toQVariant());
  QVariant qv2(v2.toQVariant());
  QVariant qv3(v3.toQVariant());

  QCOMPARE(qv1.userType(), qMetaTypeId<int>());
  QCOMPARE(qv1.toInt(), 123);

  QCOMPARE(qv2.userType(), qMetaTypeId<QString>());
  QCOMPARE(qv2.toString(), QString("abc"));

  QCOMPARE(qv3.userType(), qMetaTypeId<PiiMatrix<int> >());
  QVERIFY(Pii::equals(qv3.value<PiiMatrix<int> >(), PiiMatrix<int>(1, 2, 2, 2)));
}

QTEST_MAIN(TestPiiVariant)
