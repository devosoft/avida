#ifndef AVD_ANALYZER_DATA_ENTRY_PROTO_HH
#define AVD_ANALYZER_DATA_ENTRY_PROTO_HH

/*
With the exception of methods added to retreive data as a QString, each
of the template classes

  avd_o_AnalyzerDataEntryBase<T>, avd_o_AnalyzerDataEntry<T,U>,
  avd_o_AnalyzerArgDataEntry<T,OUT,ARG>, and
  avd_o_AnalyzerDataEntryCommand<T>

is identical to the corresponding class

  tDataEntryBase<T>, tDataEntry<T,U>, tArgDataEntry<T,OUT,ARG>, or
  tDataEntryCommand<T>

defined in tools/data_entry.hh.

avd_o_AnalyzerDataEntryBase<T> objects, and avd_o_AnalyzerDataEntryBase<T>
subclass objects, can be interchanged with tDataEntryBase<T> objects.
*/

template <class T> class avd_o_AnalyzerDataEntryBase
: public tDataEntryBase<T>
{
public:
  avd_o_AnalyzerDataEntryBase(
    const cString & _name,
    const cString & _desc,
    const cString & _null="0",
    const cString & _html_cell="align=center"
  ):tDataEntryBase<T>(_name, _desc, _null, _html_cell)
  {}
  virtual QString text(void) { return QString::null; }
};

/*
Since partial specialization of avd_o_AnalyzerDataEntry<T,U>::text() isn't
allowed, we use this helper class to specialize for converting cStrings
to QStrings in text().
*/
template <class U> class avd_o_AnalyzerDataEntryPrintHelper{
public:
  QString text(U data){
    return QString("%1").arg(data);
  }
};
template <> QString
avd_o_AnalyzerDataEntryPrintHelper<cString>::text(cString data){
  return QString(data);
}
template <> QString
avd_o_AnalyzerDataEntryPrintHelper<const cString &>::text(const cString &data){
  return QString(data);
}

template <class T, class OUT> class avd_o_AnalyzerDataEntry
: public avd_o_AnalyzerDataEntryBase<T>
{
protected:
  OUT  (T::*DataRetrieval)() const;
  void (T::*DataSet)(OUT);
  int  (T::*DataCompare)(T*) const;
  int CmpNULL(T *) const { return 0; }
  avd_o_AnalyzerDataEntryPrintHelper<OUT> m_printhelper;
public:
  avd_o_AnalyzerDataEntry(
    const cString & _name,
    const cString & _desc,
    OUT (T::*_funR)() const,
    void (T::*_funS)(OUT _val) = NULL,
    int (T::*_funC)(T * _o) const = 0,
    const cString & _null="0",
    const cString & _html_cell="align=center"
  ):avd_o_AnalyzerDataEntryBase<T>(_name, _desc, _null, _html_cell),
    DataRetrieval(_funR),
    DataSet(_funS),
    DataCompare(_funC)
  {}
  bool Print(std::ostream & fp) const {
    if (target == NULL) return false;
    fp << (target->*DataRetrieval)();
    return true;
  }
  int Compare(T * other) const {
    return (DataCompare)?((target->*DataCompare)(other)):(0);
  }
  bool Set(const cString & value) {
    OUT new_value(0);
    if (DataSet == 0) return false;
    (target->*DataSet)( cStringUtil::Convert(value, new_value) );
    return true;
  }
  QString text(void) {
    if (target == 0) return QString::null;
    else return m_printhelper.text((target->*DataRetrieval)());
  }
};

template <class T, class OUT, class ARG> class avd_o_AnalyzerArgDataEntry
  : public avd_o_AnalyzerDataEntryBase<T> {
protected:
  OUT (T::*DataRetrieval)(ARG) const;
  int (T::*DataCompare)(T*,ARG) const;
  ARG arg;
  avd_o_AnalyzerDataEntryPrintHelper<OUT> m_printhelper;
public:
  avd_o_AnalyzerArgDataEntry(
    const cString & _name,
    const cString & _desc,
    OUT (T::*_funR)(ARG) const, ARG _arg,
    int (T::*_funC)(T * _o, ARG _i) const = 0,
    const cString & _null="0",
    const cString & _html_cell="align=center"
  ):avd_o_AnalyzerDataEntryBase<T>(_name, _desc, _null, _html_cell),
    DataRetrieval(_funR),
    DataCompare(_funC),
    arg(_arg)
  {}
  bool Print(std::ostream & fp) const {
    if (target == NULL) return false;
    fp << (target->*DataRetrieval)(arg);
    return true;
  }
  int Compare(T * other) const {
    return (DataCompare)?((target->*DataCompare)(other, arg)):(0);
  }
  QString text(void) {
    return m_printhelper.text((target->*DataRetrieval)(arg));
  }
};


template <class T> class avd_o_AnalyzerDataEntryCommand {
protected:
  avd_o_AnalyzerDataEntryBase<T> * data_entry;
  cStringList args;
public:
  avd_o_AnalyzerDataEntryCommand(
    avd_o_AnalyzerDataEntryBase<T> * _entry,
    const cString & _args=""
  ):data_entry(_entry),
    args(_args, ':')
  {}
  avd_o_AnalyzerDataEntryBase<T> & GetEntry() { return *data_entry; }
  const cStringList & GetArgs() const { return args; }
  bool HasArg(const cString & test_arg) { return args.HasString(test_arg); }
  const cString & GetName() const { return data_entry->GetName(); }
  const cString & GetDesc() const { return data_entry->GetDesc(); }
  const cString & GetNull() const { return data_entry->GetNull(); }
  const cString & GetHtmlCellFlags() const
    { return data_entry->GetHtmlCellFlags(); }
  void SetTarget(T * _target) { data_entry->SetTarget(_target); }
  bool Print(std::ostream & fp) const { return data_entry->Print(fp); }
  int Compare(T * other) const { return data_entry->Compare(other); }
  bool SetValue(const cString & value) { return data_entry->Set(value); }
  void HTMLPrint(std::ostream & fp, int compare=0, bool print_text=true)
    { data_entry->HTMLPrint(fp, compare, print_text); }
  QString text(void) const {
    return data_entry->text();
  }
};


#endif /* AVD_ANALYZER_DATA_ENTRY_PROTO_HH */

// arch-tag: proto file for old analyzer data entry objects
