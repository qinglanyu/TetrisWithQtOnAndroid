package android-sources;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.View;

public class QtFullScreenActivity
{
    static final private String TAG = "QtFullScreenActivity";
    static private QtFullScreenActivity mInstance = new QtFullScreenActivity();
    private Context mContext = null;

    private QtFullScreenActivity() { }

    static public QtFullScreenActivity instance() { return mInstance; }

    public void setContext(Context context) {
        Log.d(TAG, "context: " + context.toString());
        mContext = context;
    }

    public void fullScreenLeanBack() {
        Log.d(TAG, "fullScreenLeanBack");
        Activity activity = (Activity) mContext;
        View decorView = activity.getWindow().getDecorView();
        decorView.setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }

    public void fullScreenImmersive() {
        Log.d(TAG, "fullScreenImmersive");
        Activity activity = (Activity) mContext;
        View decorView = activity.getWindow().getDecorView();
        decorView.setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }

    public void fullScreenStickyImmersive() {
        Log.d(TAG, "fullScreenStickyImmersive");
        Activity activity = (Activity) mContext;
        View decorView = activity.getWindow().getDecorView();
        decorView.setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }

    public int addTest(int value0, int value1) {
        Log.d(TAG, "I'm a test.");
        return value0 + value1;
    }
}


