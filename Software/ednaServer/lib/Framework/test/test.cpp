#include <unity.h>

#include <KPController.hpp>
#include <KPFileLoader.hpp>
#include <KPSerialInput.hpp>
#include <KPServer.hpp>
#include <KPStateMachine.hpp>
#include <KPApplicationRuntime.hpp>

void TestStringConstructors() {
	println("TESTING: TestStringConstructors");

	char name[] = "Kawin";
	KPStringBuilder<100> a("Hi there ", name, ", how are you doing today?");
	TEST_ASSERT(a == "Hi there Kawin, how are you doing today?");

	KPStringBuilder<100> b = KPStringBuilder<400>("123456789");
	TEST_ASSERT(b.length() == 9);
	TEST_ASSERT(b == "123456789");

	KPStringBuilder<9> d("1", 2, "3", 4, "5", 6, "7", 8, "9", KPStringBuilder<5>("xxxxx"));
	TEST_ASSERT(d.length() == 9);
	TEST_ASSERT(d == "123456789");

	KPStringBuilder<9> e;
	e = "123456789xxxxx";
	TEST_ASSERT(e.length() == 9);
	TEST_ASSERT(e == "123456789");

	e = a;
	TEST_ASSERT(e.length() == 9);
	TEST_ASSERT(e == "Hi there ");
	TEST_ASSERT(strcmp(KPStringBuilder<5>("12345").c_str(), "12345") == 0);

	KPStringBuilder<12> f{"Hi there"};
	TEST_ASSERT(f == "Hi there");
}

void setup() {
	delay(5000);
	UNITY_BEGIN();
	RUN_TEST(TestStringConstructors);
	UNITY_END();
}

void loop() {
}