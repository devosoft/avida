tell application id "com.google.Chrome"
	repeat with w in every window
		repeat with t in (tabs of w)
			if (title of t = "Avida-ED") then
				tell t to reload
			end if
		end repeat
	end repeat
end tell


