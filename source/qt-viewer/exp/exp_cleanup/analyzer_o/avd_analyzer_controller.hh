#ifndef AVD_ANALYZER_CONTROLLER_HH
#define AVD_ANALYZER_CONTROLLER_HH


class QMenuBar;
class avd_o_AnalyzerCtrlData;
class avdAvidaThreadDrvr;
class avdMCMediator;
class avd_o_AnalyzerCtrl : public QObject, public avdAbstractCtrl
{
private:
  avd_o_AnalyzerCtrlData *d;
public:
  avd_o_AnalyzerCtrl();
  ~avd_o_AnalyzerCtrl();
  bool setup(avdMCMediator *mediator);
};

#endif

// arch-tag: header file for old analyzer gui controller
