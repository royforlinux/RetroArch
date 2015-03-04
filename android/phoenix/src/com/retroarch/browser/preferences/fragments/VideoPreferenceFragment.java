package com.retroarch.browser.preferences.fragments;

import com.retroarch.R;
import com.retroarch.browser.preferences.fragments.util.PreferenceListFragment;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.preference.Preference.OnPreferenceClickListener;
import android.view.Display;
import android.view.WindowManager;
import android.widget.Toast;

/**
 * A {@link PreferenceListFragment} responsible for handling the video preferences.
 */
public final class VideoPreferenceFragment extends PreferenceListFragment implements OnPreferenceClickListener
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// Add preferences from the resources
		addPreferencesFromResource(R.xml.video_preferences);

		// Set preference click listeners
		findPreference("set_os_reported_ref_rate_pref").setOnPreferenceClickListener(this);
	}

	@Override
	public boolean onPreferenceClick(Preference preference)
	{
		final String prefKey = preference.getKey();

		// Set OS-reported refresh rate preference.
		if (prefKey.equals("set_os_reported_ref_rate_pref"))
		{
			final WindowManager wm = getActivity().getWindowManager();
			final Display display = wm.getDefaultDisplay();
			final double rate = display.getRefreshRate();

			final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getActivity());
			final SharedPreferences.Editor edit = prefs.edit();
			edit.putString("video_refresh_rate", Double.toString(rate));
			edit.apply();

			Toast.makeText(getActivity(), String.format(getString(R.string.using_os_reported_refresh_rate), rate), Toast.LENGTH_LONG).show();
		}

		return true;
	}
}
