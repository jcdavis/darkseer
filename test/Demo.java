import is.jcdav.darkseer.DarkSeer;

/* A Demo that shows the change in allocations as a return of string conat
 * optimization in c2. Note that this is no longer interesting as of java 9+,
 * since indy concat in means that the conccat is optimal immediately :)
 */
public class Demo {
  public static String test(String s1, String s2) {
    return s1 + s2;
  }

  public static void main(String[] args) {
    test("hello", "world");
    DarkSeer.start();
    test("hello", "world");
    DarkSeer.end();
    for(int i = 0; i < 500000; i++) {
      test("hello", "world");
    }
    DarkSeer.start();
    test("hello", "world");
    DarkSeer.end();
  }
}
