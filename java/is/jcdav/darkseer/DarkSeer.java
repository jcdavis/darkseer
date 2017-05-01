package is.jcdav.darkseer;

import java.util.Arrays;

public class DarkSeer {
  public static native void start();
  /**
   * 0 print no object data
   * 1 print primitives and arrays only (default)
   * 2 print every object
   *
   * These are not stable and may change
   */
  public static native void end(int level);
  public static void end() {
    end(1);
  }

  //Used to jailbreak out of JNI for printing logic.
  private static void printValue(Object object, int level) {
    Class clazz = object.getClass();
    if ((clazz.isPrimitive() || clazz.isArray()) && level > 0) {
      if (object instanceof boolean[]) {
        System.out.println(Arrays.toString((boolean[]) object));
      } else if (object instanceof byte[]) {
        System.out.println(Arrays.toString((byte[]) object));
      } else if (object instanceof short[]) {
        System.out.println(Arrays.toString((short[]) object));
      } else if (object instanceof char[]) {
        System.out.println(Arrays.toString((char[]) object));
      } else if (object instanceof int[]) {
        System.out.println(Arrays.toString((int[]) object));
      } else if (object instanceof float[]) {
        System.out.println(Arrays.toString((float[]) object));
      } else if (object instanceof long[]) {
        System.out.println(Arrays.toString((long[]) object));
      } else if (object instanceof double[]) {
        System.out.println(Arrays.toString((double[]) object));
      } else if (object instanceof Object[]) {
        System.out.println(Arrays.deepToString((Object[]) object));
      } else {
        System.out.println(object.toString());
      }
    } else if (level == 2) {
      System.out.println(object.toString());
    }
  }

  // Warmup to clear out initializaion allocations
  static {
    start();
    end();
  }
}
