#
# Dart server to submit results (used by client)
#
SET (DROP_METHOD "scp")
SET (DROP_SITE "toto.mmg.msu.edu")
SET (DROP_LOCATION "incoming")
SET (DROP_SITE_USER "dummy")
SET (TRIGGER_SITE "http://${DROP_SITE}/cgi-bin/dart--avida--current.pl")

# Problem build email delivery variables
SET (DELIVER_BROKEN_BUILD_EMAIL "Continuous")
SET (EMAIL_FROM "avida.current.dashboard@stochastic.net")
SET (DARTBOARD_BASE_URL "http://toto.mmg.msu.edu/avida-dashboards/current/build/Testing/HTML/TestingResults")
SET (EMAIL_PROJECT_NAME "Avida 2.0")
SET (BUILD_MONITORS "{.* avida.current.dashboard.monitors@stochastic.net}")
SET (CVS_IDENT_TO_EMAIL "{kaben kaben.toto.mmg.msu.edu@kaben.stochastic.net} {brysonda brysonda.toto.mmg.msu.edu@kaben.stochastic.net} {jclune jclune.toto.mmg.msu.edu@kaben.stochastic.net} {dule123 dule123.toto.mmg.msu.edu@kaben.stochastic.net} {hagstrom hagstrom.toto.mmg.msu.edu@kaben.stochastic.net} {bdbaer bdbaer.toto.mmg.msu.edu@kaben.stochastic.net} {mercere99 mercere99.toto.mmg.msu.edu@kaben.stochastic.net} {covertar covertar.toto.mmg.msu.edu@kaben.stochastic.net} {ostrow24 ostrow24.toto.mmg.msu.edu@kaben.stochastic.net} {avidacrashdummy avidacrashdummy.toto.mmg.msu.edu@kaben.stochastic.net} {huangw10 huangw10.toto.mmg.msu.edu@kaben.stochastic.net} {goingssh goingssh.toto.mmg.msu.edu@kaben.stochastic.net} {wilke wilke.toto.mmg.msu.edu@kaben.stochastic.net} {stredwjm stredwjm.toto.mmg.msu.edu@kaben.stochastic.net}")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_CONFIGURE_FAILURES "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_BUILD_ERRORS "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_BUILD_WARNINGS "0")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_TEST_NOT_RUNS "1")
SET (DELIVER_BROKEN_BUILD_EMAIL_WITH_TEST_FAILURES "1")

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/AvidaLogo.gif 
  ${PROJECT_BINARY_DIR}/Testing/HTML/TestingResults/Icons/Logo.gif
  COPYONLY
)
