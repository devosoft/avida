tell application id "com.google.Chrome"
	repeat with w in every window
		repeat with t in (tabs of w)
			if (title of t = "Avida-ED") then
				tell t to reload
			end if
		end repeat
	end repeat
end tell

tell application id "com.apple.Safari"
	repeat with w in (get windows whose its document is not missing value)
		repeat with t in (get tabs of w)
			if (name of t = "Avida-ED") then
				do JavaScript "document.location.reload(true)" in t
			end if
		end repeat
	end repeat
end tell
