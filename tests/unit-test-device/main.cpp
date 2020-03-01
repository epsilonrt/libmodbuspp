// libmodbuspp Unit Test template
// Use UnitTest++ framework -> https://github.com/unittest-cpp/unittest-cpp/wiki
// This test code is in the public domain.
#include <iostream>
#include <modbuspp.h>
#include <UnitTest++/UnitTest++.h>

using namespace std;
using namespace Modbus;

////////////////////////////////////////////////////////////////////////////////
// 
// To add a test, simply put the following code in the a .cpp file of your choice:
//
// =================================
// Simple Test
// =================================
//
//  TEST(YourTestName)
//  {
//  }
//
// The TEST macro contains enough machinery to turn this slightly odd-looking 
// syntax into legal C++, and automatically register the test in a global list.
// This test list forms the basis of what is executed by RunAllTests().
// e.g.
TEST (RtuMasterTest) {
  string connection ("/dev/ttyUSB0");
  string settings ("38400E1");
  Net net = Rtu;
  int slvAddr = 33;

  Master mb; // instantiate new MODBUS Master
  CHECK (!mb.isValid());

  // set master on RTU
  CHECK (mb.setBackend (net, connection, settings));
  CHECK (mb.isValid());
  CHECK (mb.net() == net);
  CHECK (mb.connection() == connection);
  CHECK (mb.settings() == settings) ;

  Slave & slv = mb.addSlave (slvAddr);
  CHECK (mb.hasSlave (slvAddr));

  REQUIRE CHECK (mb.open ());
  uint16_t value;
  CHECK (slv.readInputRegisters (1, &value) == 1);

  cout << "R0=" << value << endl;
  mb.close();
  CHECK (!mb.isOpen());
}

//
// If you want to re-use a set of test data for more than one test, or provide 
// setup/teardown for tests, you can use the TEST_FIXTURE macro instead. 
// The macro requires that you pass it a class name that it will instantiate, 
// so any setup and teardown code should be in its constructor and destructor.
//
//  struct SomeFixture
//  {
//    SomeFixture() { /* some setup */ }
//    ~SomeFixture() { /* some teardown */ }
//
//    int testData;
//  };
//
//  TEST_FIXTURE(SomeFixture, YourTestName)
//  {
//    int temp = testData;
//  }
//
// =================================
// Test Suites
// =================================
//
// Tests can be grouped into suites, using the SUITE macro. A suite serves as a 
// namespace for test names, so that the same test name can be used in two 
// difference contexts.
//
//  SUITE(YourSuiteName)
//  {
//    TEST(YourTestName)
//    {
//    }
//
//    TEST(YourOtherTestName)
//    {
//    }
//  }
//
// This will place the tests into a C++ namespace called YourSuiteName, and make 
// the suite name available to UnitTest++.
// RunAllTests() can be called for a specific suite name, so you can use this to 
// build named groups of tests to be run together.
// Note how members of the fixture are used as if they are a part of the test, 
// since the macro-generated test class derives from the provided fixture class.
//
//
////////////////////////////////////////////////////////////////////////////////

// run all tests
int main (int argc, char **argv) {
  return UnitTest::RunAllTests();
}

/* ========================================================================== */
