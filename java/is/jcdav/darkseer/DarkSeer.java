package is.jcdav.darkseer;

public class DarkSeer {
  public static native void start();
  public static native void end(boolean printValues);
  public static void end() {
    end(false);
  }

  //Used to jailbreak out of JNI for printing logic.
  private static void printValue(Object obj) {
    System.out.println(obj.toString());
  }

  // Warmup to clear out initializaion allocations
  static {
    start();
    end();
  }
}
