package is.jcdav.darkseer;

public class DarkSeer {
  public static native void start();
  public static native void end();

  // Warmup to clear out initializaion allocations
  static {
    start();
    end();
  }
}
