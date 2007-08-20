#include "cBitArray.h"

void cRawBitArray::Copy(const cRawBitArray & in_array, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  if (bit_fields != NULL) {
    delete [] bit_fields;
  }
  bit_fields = new int[num_fields];
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = in_array.bit_fields[i];
  }
}


bool cRawBitArray::IsEqual(const cRawBitArray & in_array, int num_bits) const
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    if (bit_fields[i] != in_array.bit_fields[i]) return false;
  }
  return true;
}


void cRawBitArray::Resize(const int old_bits, const int new_bits)
{
  const int num_old_fields = GetNumFields(old_bits);
  const int num_new_fields = GetNumFields(new_bits);
  if (num_old_fields == num_new_fields) {
    // Clear all bits past the new end and stop.
    int & last_field = bit_fields[num_new_fields - 1];
    for (int i = new_bits; i < old_bits; i++) {
      const int clear_bit = i & 31;
      last_field &= ~(1 << clear_bit);
    }
    return;
  }

  // If we made it this far, we have to change the number of fields.
  // Create the new bit array and copy the old one into it.
  int * new_bit_fields = new int[ num_new_fields ];
  for (int i = 0; i < num_new_fields && i < num_old_fields; i++) {
    new_bit_fields[i] = bit_fields[i];
  }
  
  // If the old bits are longer, we need to clear the end of the last
  // bit field.
  if (num_old_fields > num_new_fields) {
    int & last_field = new_bit_fields[num_new_fields - 1];
    for (int clear_bit=GetFieldPos(new_bits); clear_bit < 32; clear_bit++) {
      last_field &= ~(1 << clear_bit);
    }
  }
  
  // If the new bits are longer, clear everything past the end of the old
  // bits.
  for (int i = num_old_fields; i < num_new_fields; i++) {
    new_bit_fields[i] = 0;
  }

  if (bit_fields != NULL) {
    delete [] bit_fields;
  }
  bit_fields = new_bit_fields;
}


void cRawBitArray::ResizeSloppy(const int new_bits)
{
  const int new_fields = GetNumFields(new_bits);
  if (bit_fields != NULL) {
    delete [] bit_fields;
  }
  bit_fields = new int[ new_fields ];
}

void cRawBitArray::ResizeClear(const int new_bits)
{
  ResizeSloppy(new_bits);
  Zero(new_bits);
}


// This technique counts the number of bits; it loops through once for each
// bit equal to 1.  This is reasonably fast for sparse arrays.
int cRawBitArray::CountBits(const int num_bits) const
{
  const int num_fields = GetNumFields(num_bits);
  int bit_count = 0;
  
  for (int i = 0; i < num_fields; i++) {
    int temp = bit_fields[i];
    while (temp != 0) {
      temp = temp & (temp - 1);
      bit_count++;
    }
  }
  return bit_count;
}

// This technique is another way of counting bits; It does a bunch of
// clever bit tricks to do it in parallel in each int.
int cRawBitArray::CountBits2(const int num_bits) const
{
  const int num_fields = GetNumFields(num_bits);
  int bit_count = 0;
  
  for (int i = 0; i < num_fields; i++) {
    const int  v = bit_fields[i];
    unsigned int const t1 = v - ((v >> 1) & 0x55555555);
    unsigned int const t2 = (t1 & 0x33333333) + ((t1 >> 2) & 0x33333333);
    bit_count += ((t2 + (t2 >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  }
  return bit_count;
}

int cRawBitArray::FindBit1(const int num_bits, const int start_pos) const
{
  // @CAO -- There are probably better ways to do this with bit tricks.
  for (int i = start_pos; i < num_bits; i++) {
    if (GetBit(i) == true) return i;
  }

  return -1;
}

tArray<int> cRawBitArray::GetOnes(const int num_bits) const
{
  // @CAO -- There are probably better ways to do this with bit tricks.
  tArray<int> out_array(CountBits2(num_bits));
  int cur_pos = 0;
  for (int i = 0; i < num_bits; i++) {
    if (GetBit(i) == true) out_array[cur_pos++] = i;
  }

  return out_array;
}


void cRawBitArray::NOT(const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~bit_fields[i];
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}

void cRawBitArray::AND(const cRawBitArray & array2, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] &= array2.bit_fields[i];
  }
}

void cRawBitArray::OR(const cRawBitArray & array2, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] |= array2.bit_fields[i];
  }
}

void cRawBitArray::NAND(const cRawBitArray & array2, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~(bit_fields[i] & array2.bit_fields[i]);
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}

void cRawBitArray::NOR(const cRawBitArray & array2, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~(bit_fields[i] | array2.bit_fields[i]);
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}

void cRawBitArray::XOR(const cRawBitArray & array2, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] ^= array2.bit_fields[i];
  }

}

void cRawBitArray::EQU(const cRawBitArray & array2, const int num_bits)
{
  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~(bit_fields[i] ^ array2.bit_fields[i]);
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}





void cRawBitArray::NOT(const cRawBitArray & array1, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~array1.bit_fields[i];
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}

void cRawBitArray::AND(const cRawBitArray & array1,
		       const cRawBitArray & array2, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = array1.bit_fields[i] & array2.bit_fields[i];
  }
}

void cRawBitArray::OR(const cRawBitArray & array1,
		      const cRawBitArray & array2, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = array1.bit_fields[i] | array2.bit_fields[i];
  }
}

void cRawBitArray::NAND(const cRawBitArray & array1,
			const cRawBitArray & array2, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~(array1.bit_fields[i] & array2.bit_fields[i]);
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}

void cRawBitArray::NOR(const cRawBitArray & array1,
		       const cRawBitArray & array2, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~(array1.bit_fields[i] | array2.bit_fields[i]);
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}

void cRawBitArray::XOR(const cRawBitArray & array1,
		       const cRawBitArray & array2, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = array1.bit_fields[i] ^ array2.bit_fields[i];
  }

}

void cRawBitArray::EQU(const cRawBitArray & array1, const cRawBitArray & array2, const int num_bits)
{
  ResizeSloppy(num_bits);

  const int num_fields = GetNumFields(num_bits);
  for (int i = 0; i < num_fields; i++) {
    bit_fields[i] = ~(array1.bit_fields[i] ^ array2.bit_fields[i]);
  }

  const int last_bit = GetFieldPos(num_bits);
  if (last_bit > 0) {
    bit_fields[num_fields - 1] &= (1 << last_bit) - 1;
  }
}


std::ostream & operator << (std::ostream & out, const cBitArray & bit_array)
{
  bit_array.Print(out);
  return out;
}



#ifdef UNITTEST_CBITARRAY

int main()
{
  int passed = true;
  // Start by testing the cRawBitArray class.

  // Test default constructor.
  cRawBitArray bit_array1;
  bit_array1.ResizeClear(10);
  for (int i = 0; i < 10; i++) {
    if (bit_array1.GetBit(i) != false) {
      passed = false;
      cerr << "ERROR in testing bit that should be cleared by ResizeClear()!"
	   << endl;
    }
  }

  //bit_array1.Print(10);
  
  bit_array1.SetBit(1, true);
  bit_array1.SetBit(3, true);
  bit_array1.SetBit(5, true);
  bit_array1.SetBit(7, true);
  bit_array1.SetBit(9, true);
  
  for (int i = 0; i < 10; i++) {
    bool bit_value = !(2*(i/2) == i);
    if (bit_array1.GetBit(i) != bit_value) {
      passed = false;
      cerr << "ERROR in testing bits after SetBit() was run on them!"
	   << endl;
    }
  }

  //bit_array1.Print(10);
  bit_array1.SetBit(0, true);
  bit_array1.SetBit(1, false);
  bit_array1.SetBit(5, false);
  bit_array1.SetBit(6, true);
  bit_array1.SetBit(7, false);
  //bit_array1.Print(10);
  
  for (int i = 0; i < 10; i++) {
    bool bit_value = (3*(i/3) == i);
    if (bit_array1.GetBit(i) != bit_value) {
      passed = false;
      cerr << "ERROR in testing bits after second round of SetBit() was run!"
	   << endl;
    }
  }
  
  // Test constructor with initial size < 32.
  cRawBitArray bit_array2(26);
  for (int i = 0; i < 26; i++) {
    if (bit_array2.GetBit(i) != false) {
      passed = false;
      cerr << "ERROR in testing bit that should be cleared by constructor()!"
	   << endl;
    }
  }
  bit_array2.SetBit(8, true);
  bit_array2.Copy(bit_array1, 10);

  for (int i = 0; i < 10; i++) {
    bool bit_value = (3*(i/3) == i);
    if (bit_array2.GetBit(i) != bit_value) {
      passed = false;
      cerr << "ERROR in testing bits after Copy()!"
	   << endl;
    }
  }
  
  // Test constructor with initial size > 32.
  const int high_bit_count = 1000;
  cRawBitArray bit_array3(high_bit_count);
  int bit_pos = 2;
  while (bit_pos < high_bit_count) {
    bit_array3.SetBit(bit_pos, true);
    bit_pos = bit_pos * 3 / 2;
  }

  // Test faux copy constructor.
  cRawBitArray bit_array4(bit_array3, high_bit_count);
  bit_array4.SetBit(22, true);
  bit_array4.SetBit(24, true);
  int count1 =  bit_array3.CountBits(high_bit_count);
  int count2 =  bit_array3.CountBits2(high_bit_count);
  int count3 =  bit_array4.CountBits(high_bit_count);
  int count4 =  bit_array4.CountBits2(high_bit_count);

  if (count1 != count2 || count3 != count4) {
    passed = false;
    cerr << "ERROR: CountBits and CountBits2 don't agree!" << endl;
  }

  if (count1 != count3 - 2) {
    passed = false;
    cerr << "ERROR: Counts do not agree before and after copy!" << endl;
  }

  int diff_count = 0;
  for (int i = 0; i < high_bit_count; i++) {
    if (bit_array3.GetBit(i) != bit_array4.GetBit(i)) diff_count++;
  }

  if (diff_count != 2) {
    passed = false;
    cerr << "ERROR in cRawBitArray copy constructor." << endl;
  }


  // LOGICAL OPERATORS
  
  bit_array4.Resize(1000, 70);
  int count5 = bit_array4.CountBits(70);
  bit_array4.NOT(70);
  int count6 = bit_array4.CountBits(70);
  bit_array4.NOT(70);
  
  if (count5 + count6 != 70) {
    passed = false;
    cerr << "ERROR in NOT operation!" << endl;
  }

  cRawBitArray bit_array5(70);
  int pos = 1;
  int step = 1;
  while (pos <= 70) {
    bit_array5.SetBit(70 - pos, true);
    pos += step++;
  }

  cRawBitArray bit_array6(70);
  bit_array6.AND(bit_array4, bit_array5, 70);
  int count_and = bit_array6.CountBits(70);
  if (count_and != 3) {
    passed = false;
    cerr << "ERROR in AND operation!" << endl;
  }

  bit_array6.OR(bit_array4, bit_array5, 70);
  int count_or = bit_array6.CountBits(70);
  if (count_or != 21) {
    passed = false;
    cerr << "ERROR in OR operation!" << endl;
  }

  bit_array6.NAND(bit_array4, bit_array5, 70);
  int count_nand = bit_array6.CountBits(70);
  if (count_nand != 67) {
    passed = false;
    cerr << "ERROR in NAND operation!" << endl;
  }

  bit_array6.NOR(bit_array4, bit_array5, 70);
  int count_nor = bit_array6.CountBits(70);
  if (count_nor != 49) {
    passed = false;
    cerr << "ERROR in NOR operation!" << endl;
  }

  bit_array6.XOR(bit_array4, bit_array5, 70);
  int count_xor = bit_array6.CountBits(70);
  if (count_xor != 18) {
    passed = false;
    cerr << "ERROR in XOR operation!" << endl;
  }

  bit_array6.EQU(bit_array4, bit_array5, 70);
  int count_equ = bit_array6.CountBits(70);
  if (count_equ != 52) {
    passed = false;
    cerr << "ERROR in EQU operation!" << endl;
  }

//   bit_array4.Print(70);
//   bit_array5.Print(70);
//   bit_array6.Print(70);
//   cout << bit_array6.CountBits(70) << endl;

  cBitArray ba(74);
  for (int i = 0; i < 74; i++) {  if (i % 5 == 3) ba[i] = true;  }

  cBitArray ba2(74);
  for (int i = 0; i < 74; i++)  {
    if ((i%2==0 || i%3==0) && i%6 != 0) ba2[i] = true;
  }

  if ((ba & ba2).CountBits() != 8) {
    passed = false;
    cerr << "ERROR: operator& failed for cBitArray" << endl;
  }
  
  if ((ba | ba2).CountBits() != 43) {
    passed = false;
    cerr << "ERROR: operator| failed for cBitArray" << endl;
  }

  if ((ba ^ ba2).CountBits() != 35) {
    passed = false;
    cerr << "ERROR: operator^ failed for cBitArray" << endl;
  }
  
  if ((~ba).CountBits() != 59) {
    passed = false;
    cerr << "ERROR: operator~ failed for cBitArray" << endl;
  }

  if ((~ba & ~ba2).CountBits() != 31) {
    passed = false;
    cerr << "ERROR: Chained bitwise operators failed for cBitArray" << endl;
  }
  
  cout << ba << "  " << ba.CountBits() << endl;
  cout << ba2 << "  " << ba2.CountBits() << endl;
  cout << (~ba & ~ba2) << "  " << (~ba & ~ba2).CountBits() << endl;

  if (passed == true) {
    cout << "cRawBitArray passed Unit Tests." << endl;
  }
}

#endif
