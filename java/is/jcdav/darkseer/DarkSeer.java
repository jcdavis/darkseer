package is.jcdav.darkseer;

import java.util.Arrays;
import java.util.List;

public class DarkSeer {
  public static native void start();
  /**
   * 0 print nothing, just return the # of bytes allocated
   * 1 print the objects, but not their contents
   * 2 print contents of primitives and arrays only (default)
   * 3 print contents of every object
   *
   * These are not stable and may change
   */
  public static native int end(int level);
  public static int end() {
    return end(2);
  }

  private static final List<Class<?>> primitives = Arrays.asList(new Class<?>[]{
    Boolean.class,
    Byte.class,
    Short.class,
    Character.class,
    Integer.class,
    Float.class,
    Long.class,
    Double.class
  });
  //Used to jailbreak out of JNI for printing logic.
  private static void printValue(Object object, int level) {
    Class clazz = object.getClass();
    if ((primitives.contains(clazz) || clazz.isArray()) && level > 1) {
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
    } else if (level >= 3) {
      System.out.println(object.toString());
    }
  }

  // Warmup to clear out initializaion allocations
  static {
    start();
    end(0);
  }
}
