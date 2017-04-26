import is.jcdav.darkseer.DarkSeer;

public class Demo {
  public static String test(String s1, String s2) {
    return s1 + s2;
  }

  public static void main(String[] args) {
    test("hello", "world");
    DarkSeer.start();
    test("hello", "world");
    DarkSeer.end(true);
    for(int i = 0; i < 500000; i++) {
      test("hello", "world");
    }
    DarkSeer.start();
    test("hello", "world");
    DarkSeer.end(true);
  }
}
