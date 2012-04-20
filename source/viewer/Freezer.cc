/*
 *  viewer/Freezer.cc
 *  Avida
 *
 *  Created by David on 1/4/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Authors: David M. Bryson <david@programerror.com>
 */

#include "avida/viewer/Freezer.h"

#include "avida/core/Genome.h"

#include "cAvidaConfig.h"
#include "cEnvironment.h"
#include "cFile.h"
#include "cPopulation.h"
#include "cWorld.h"


namespace Avida {
  namespace Viewer {
    namespace Private {
      
      void WriteInstSet(const Apto::String& path)
      {
        cFile file;
        std::fstream* fs;
        
        Apto::String file_path = Apto::FileSystem::PathAppend(path, "instset.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "INSTSET heads_default:hw_type=0" << std::endl;
        *fs << "" << std::endl;
        *fs << "# No-ops" << std::endl;
        *fs << "INST nop-A         # a" << std::endl;
        *fs << "INST nop-B         # b" << std::endl;
        *fs << "INST nop-C         # c" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Flow control operations" << std::endl;
        *fs << "INST if-n-equ      # d" << std::endl;
        *fs << "INST if-less       # e" << std::endl;
        *fs << "INST if-label      # f" << std::endl;
        *fs << "INST mov-head      # g" << std::endl;
        *fs << "INST jmp-head      # h" << std::endl;
        *fs << "INST get-head      # i" << std::endl;
        *fs << "INST set-flow      # j" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Single Argument Math" << std::endl;
        *fs << "INST shift-r       # k" << std::endl;
        *fs << "INST shift-l       # l" << std::endl;
        *fs << "INST inc           # m" << std::endl;
        *fs << "INST dec           # n" << std::endl;
        *fs << "INST push          # o" << std::endl;
        *fs << "INST pop           # p" << std::endl;
        *fs << "INST swap-stk      # q" << std::endl;
        *fs << "INST swap          # r" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Double Argument Math" << std::endl;
        *fs << "INST add           # s" << std::endl;
        *fs << "INST sub           # t" << std::endl;
        *fs << "INST nand          # u" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Biological Operations" << std::endl;
        *fs << "INST h-copy        # v" << std::endl;
        *fs << "INST h-alloc       # w" << std::endl;
        *fs << "INST h-divide      # x" << std::endl;
        *fs << "" << std::endl;
        *fs << "# I/O and Sensory" << std::endl;
        *fs << "INST IO            # y" << std::endl;
        *fs << "INST h-search      # z" << std::endl;
        
        file.Close();
      }
      
      void WriteDefaultConfig(const Apto::String& path, const Apto::String& name)
      {
        Apto::String file_path;
        cFile file;
        std::fstream* fs;
        
        
        // avida.cfg
        file_path = Apto::FileSystem::PathAppend(path, "avida.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "WORLD_X 60" << std::endl;
        *fs << "WORLD_Y 60" << std::endl;
        *fs << "COPY_MUT_PROB 0.0075" << std::endl;
        *fs << "BIRTH_METHOD 0" << std::endl;
        *fs << "RANDOM_SEED 0" << std::endl;
        *fs << "#include instset.cfg" << std::endl;
        
        file.Close();

        
        // instset.cfg
        WriteInstSet(path);
        
        
        // events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        file.Close();

        
        // environment.cfg
        file_path = Apto::FileSystem::PathAppend(path, "environment.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "REACTION  NOT  not   process:value=1.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NAND nand  process:value=1.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  AND  and   process:value=2.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ORN  orn   process:value=2.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  OR   or    process:value=3.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ANDN andn  process:value=3.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NOR  nor   process:value=4.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  XOR  xor   process:value=4.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  EQU  equ   process:value=5.0:type=pow  requisite:max_count=1" << std::endl;
        
        file.Close();        

        
        // entryname.txt
        file_path = Apto::FileSystem::PathAppend(path, "entryname.txt");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << name << std::endl;
        file.Close();
      }
      
      
      void WriteDefaultGenome(const Apto::String& path)
      {
        Apto::String file_path = Apto::FileSystem::PathAppend(path, "genome.seq");
        cFile file;
        file.Open((const char*)file_path, std::ios::out);
        std::fstream* fs = file.GetFileStream();
        *fs << "0,heads_default,wzcagcccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
        file.Close();
        
        // entryname.txt
        file_path = Apto::FileSystem::PathAppend(path, "entryname.txt");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << "@ancestor" << std::endl;
        file.Close();
      }
      
      
      void WriteExamplePopulation(const Apto::String& path, const Apto::String& example_key)
      {
        Apto::String file_path = Apto::FileSystem::PathAppend(path, "detail.spop");
        cFile file;
        file.Open((const char*)file_path, std::ios::out);
        std::fstream* fs = file.GetFileStream();
        
        *fs << "#filetype genotype_data" << std::endl;
        *fs << "#format id src src_args parents num_units total_units length merit gest_time fitness gen_born update_born update_deactivated depth hw_type inst_set sequence cells gest_offset lineage" << std::endl;

        if (example_key == "@example") {
          *fs << "1 div:ext (none) (none) 1 1 100 0 0 0 0 -1 -1 0 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 1770 66 0" << std::endl;
        } else if (example_key == "Update200") {
          *fs << "254 div:int (none) 179 1 3 98 95 378 0.251323 0 -1 -1 6 0 heads_default wzcagcccccccccccccccjccccccccccccccccccccxcccyccccccccccccccccccccccccbccccccccccdmcccccczvfcaxgab 2370 338 0" << std::endl;
          *fs << "507 div:int (none) 243 1 1 99 95 377 0.251193 0 -1 -1 5 0 heads_default wzcagccccccccccxccccccccccccccccccvcccccccccccccccmcccccccccccccccccccccmccccccccccccccccczvfcaxgab 2303 375 0" << std::endl;
          *fs << "438 div:int (none) 359 1 2 99 96 379 0.253298 0 -1 -1 5 0 heads_default wzcagccdcccccccccqcccccccccccccccmccccccccccccccvccccccccccccccccccccccccccccccccccucccccczvfcaxgab 2068 330 0" << std::endl;
          *fs << "622 div:int (none) 283 1 1 98 85 369 0.230352 0 -1 -1 6 0 heads_default wzcagcfccccccccccccccccccccccccccccccccccccccccccccccccccqccccccccccccccciucccccccccccccczvfcaxgab 2065 132 0" << std::endl;
          *fs << "576 div:int (none) 482 1 1 100 49 335 0.251295 0 -1 -1 6 0 heads_default cccccccccccccccckcccccccccccccccccccccccnzvfyaxgabwzcagcccccccccccccccyccccccccccsccccwccccccccccccc 2017 129 0" << std::endl;
          *fs << "162 div:int (none) 67 3 3 99 96 382 0.250218 0 -1 -1 4 0 heads_default wzcagcccccccccccccccycccccccyccccccccccccccccccccccccccccccccccccccccccccccccccccclccccccnzvfcaxgab 1837,1896,1897 189,189,189 0,0,0" << std::endl;
          *fs << "553 div:int (none) 124 1 1 98 95 379 0.250189 0 -1 -1 5 0 heads_default wzcagcccccocccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccsccccacccccccccczvfcaxgab 1583 264 0" << std::endl;
          *fs << "116 div:int (none) 8 2 8 98 95 379 0.250385 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccccccccccccccccccccccccacccccrcccczvfcaxgab 1582,1643 362,330 0,0" << std::endl;
          *fs << "24 div:int (none) 1 4 9 100 97 389 0.249357 0 -1 -1 1 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccacccccccccccccccccccbcccccccccccccccccccccccccczvfcaxgab 1530,1591,1651,1652 198,197,165,131 0,0,0,0" << std::endl;
          *fs << "461 div:int (none) 194 1 1 100 97 385 0.251458 0 -1 -1 5 0 heads_default wzcagccccccccuccccccccccccccccccccccccccccbccccccccccccccccbcccccccccccccccccncdccccnccccccvvfcaxgab 1412 627 0" << std::endl;
          *fs << "530 div:int (none) 41 1 1 100 97 386 0.250925 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccucccccccycccccqccccrzvfcaxgab 1409 337 0" << std::endl;
          *fs << "369 div:int (none) 94 4 4 99 96 379 0.252398 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccscccccccccccpccccccccmcicccccccccsccccccccccucccccccccccccqccccczvfcaxgab 1174,1231,1233,1292 264,264,296,263 0,0,0,0" << std::endl;
          *fs << "612 div:int (none) 197 1 1 97 91 373 0.243875 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccccccccrccyccccccccccjcfcccczccbcccccccccccccccccccccccccacccccccccczvfcxgab 2123 192 0" << std::endl;
          *fs << "359 div:int (none) 39 1 1 99 96 379 0.251309 0 -1 -1 4 0 heads_default wzcagccdcccccccccccccccccccccccccmccccccccccccccvccccccccccccccccccccccccccccccccccucccccczvfcaxgab 2069 329 0" << std::endl;
          *fs << "497 div:int (none) 229 1 1 100 83 373 0.230159 0 -1 -1 3 0 heads_default twcagcccczcccccccccccccccczcccccccccccccccccccccccccccccfccccctcccccccccfccccccccccccccqccczvfcaxgab 1949 419 0" << std::endl;
          *fs << "428 div:int (none) 246 1 1 99 96 379 0.251989 0 -1 -1 6 0 heads_default wzcagccccccccccqccccccccccccrcccccccccccccccccdcccccccccccccccccvccccccbccccccracccccccccczvfcaxgab 1941 329 0" << std::endl;
          *fs << "106 div:int (none) 40 2 6 100 87 377 0.230388 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccfcccccccccccccckccccccccccccccccccczvfcaxgab 1773,1774 98,131 0,0" << std::endl;
          *fs << "336 div:int (none) 23 1 4 98 95 379 0.25022 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccccicccccccccccccccccccccccccccccccccccicccccccccccjzvfcaxgab 1706 363 0" << std::endl;
          *fs << "589 div:int (none) 20 1 1 101 97 387 0.250608 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccceccccccccccccsccccccccccccccccccccccccccccccccccccccccccccmccczvfcaxgab 1598 231 0" << std::endl;
          *fs << "83 div:int (none) 12 2 7 100 97 387 0.250529 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccccccceccccccccccccccccccccccccccccmccccccccccccccccccccccccccccccczvfcaxgab 1537,1539 263,230 0,0" << std::endl;
          *fs << "37 div:int (none) 25 2 5 100 97 386 0.251151 0 -1 -1 3 0 heads_default wzcagcccdccccccccccccccccccccdccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccccczvfcaxgab 1535,1595 231,197 0,0" << std::endl;
          *fs << "60 div:int (none) 27 1 4 100 97 385 0.251803 0 -1 -1 3 0 heads_default wzcagcccdcccccccccccccmcccccccccccccccccccpccccccccccccccccccccccccpccccccccccccccccccccccczvfcaxgab 1534 296 0" << std::endl;
          *fs << "152 div:int (none) 41 1 1 100 49 335 0.251295 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccucccccccccccccqccccczvhcaxgab 1408 923 0" << std::endl;
          *fs << "566 div:int (none) 363 1 1 99 96 380 0.252632 0 -1 -1 8 0 heads_default wzcagcccdccqccccccccwycccccccdccccccccccccccccccccccccgccccccnccccccccaccccccccxccccscccpczvfcaxgab 1300 269 0" << std::endl;
          *fs << "543 div:int (none) 148 1 1 100 97 384 0.252522 0 -1 -1 6 0 heads_default wzcagccxcccccqccccccccccccccccccccgcccpcccccccccccccccccccccsccccccccccuccaccccccccdcqccccczvfcaxgab 1285 268 0" << std::endl;
          *fs << "474 div:int (none) 148 2 2 100 97 383 0.252604 8 -1 -1 6 0 heads_default wzcagccccccccqccccccccccccccccccccgcccpcccccccccccccccccccccsccccccccccuccacccsccccdcqccccczvfcaxgab 1223,1284 264,263 0,0" << std::endl;
          *fs << "597 div:int (none) 296 1 1 98 95 375 0.251551 0 -1 -1 8 0 heads_default wzcagcctccccccrccccccccccccccpccccgccccaccccaccccccccclcccccccccccucccccccccccccccucccccczvfcaxgab 2317 264 0" << std::endl;
          *fs << "620 div:int (none) 495 1 1 100 85 368 0.230352 0 -1 -1 7 0 heads_default aawzcagcfccccccccccccccccccccccchccccicccccccccxcccccccccccqccccccccccccccciccccccccccccccczvfcaxgab 2125 130 0" << std::endl;
          *fs << "482 div:int (none) 389 1 1 100 49 335 0.251295 0 -1 -1 5 0 heads_default wzcagcccccccccccccccyccccccccccsccccwccccccccccccccccccccccccccccckcccccccccccccccccccccccnzvfyaxgab 1957 116 0" << std::endl;
          *fs << "298 div:int (none) 162 2 4 99 96 382 0.251309 0 -1 -1 5 0 heads_default wzcagcccccccccccccccycccccccycccccccccccccccccccccccccccccccccccccccccccccccccccxclccccccnzvfcaxgab 1898,2020 207,240 0,0" << std::endl;
          *fs << "390 div:int (none) 162 1 2 98 95 379 0.250984 0 -1 -1 5 0 heads_default wzcagcccccccccccccccycccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 1839 222 0" << std::endl;
          *fs << "183 div:int (none) 80 1 2 100 97 386 0.232804 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccpccccccccccccccccccccccccccccccccccccccccccccccccczclyccccccccccccczvfcaxgab 1832 270 0" << std::endl;
          *fs << "229 div:int (none) 40 2 4 100 83 373 0.228631 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccczcccccccccccccccccccccccccccccfccccctcccccccccccccccccccccccccccczvfcaxgab 1829,1890 33,32 0,0" << std::endl;
          *fs << "505 div:int (none) 314 1 1 99 96 383 0.250653 0 -1 -1 7 0 heads_default whcagcccccccccccccccccccccckrcccccccccccccccwcccccccccccccccccccccbcclcccccccccccccccccccczvfcaxgab 1767 363 0" << std::endl;
          *fs << "413 div:int (none) 326 1 1 98 95 376 0.25 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccceccccrcccccccctccccccccccccccccccncccccccccbcccccccccccacccccecccczvfcaxgab 1760 329 0" << std::endl;
          *fs << "367 div:int (none) 297 1 1 98 95 378 0.251309 0 -1 -1 7 0 heads_default wzcmgcccccccccccccccyoccccccyccccccqccccccccccccccccccccccccccccccccccccccccccccclccccccnzvfcaxgab 1720 997 0" << std::endl;
          *fs << "528 div:int (none) 116 1 1 98 95 379 0.25036 0 -1 -1 5 0 heads_default wzgagcccccccucccccccccccccccrcccccccccccccccccccccccccccccccccccccccccccccccccacccccrcccczvfcaxgab 1704 363 0" << std::endl;
          *fs << "321 div:int (none) 116 3 4 98 95 378 0.250881 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccccccccccccccccmcccccccacccccrcccczvfcaxgab 1400,1460,1521 363,377,377 0,0,0" << std::endl;
          *fs << "574 div:int (none) 94 1 1 100 97 385 0.251744 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccscccccccccccpcccccccccccccccccccccscccccckcccucccccccccccpcqccccczvfcaxgab 1351 264 0" << std::endl;
          *fs << "551 div:int (none) 452 1 1 199 97 383 0.252604 0 -1 -1 7 0 heads_default aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaawzcagccccccccscccccccccccccccccdcccccccccgbcccccccccccccccccccccchcccccccccccncdccccncccpccczvfcaxgab 1232 297 0" << std::endl;
          *fs << "344 div:int (none) 41 3 3 100 97 384 0.251731 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccscccccccdccucccccccccccccqccclczvfcaxgab 1227,1288,1348 297,329,263 0,0,0" << std::endl;
          *fs << "393 div:int (none) 304 5 5 99 94 379 0.248021 0 -1 -1 6 0 heads_default wzcagcccccccccccccccyccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccicccczcccnzvfcaxgab 1901,1960,2018,2019,2079 149,182,116,149,182 0,0,0,0,0" << std::endl;
          *fs << "623 div:int (none) 106 1 1 100 87 377 0.230333 0 -1 -1 3 0 heads_default wzcagpccccccccccccccccccccccccccccccccccccccccccccccccccfcccccccccccccckccccccccccccccccccczvfcaxgab 1834 132 0" << std::endl;
          *fs << "577 div:int (none) 219 1 1 99 96 383 0.250327 0 -1 -1 5 0 heads_default wzcagccccccocccccccccccccccccccccccccccccccccacccccccccccccccccccccccncccccccccccccucccccczvfcaxgab 1833 231 0" << std::endl;
          *fs << "232 div:int (none) 168 2 4 98 94 377 0.249337 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccacrccccbccccccccccccccccccccccncccccccccccccccccccccacxcccecccczvfcaxgab 1820,1821 32,33 0,0" << std::endl;
          *fs << "508 div:int (none) 408 1 1 98 95 375 0.251989 0 -1 -1 6 0 heads_default wzcagcdcccccccccccccccccecccccwccccctcfcicccccccccccccucccccccccccccccccccccccccccecccccczvfcaxgab 1465 363 0" << std::endl;
          *fs << "209 div:int (none) 103 4 5 99 96 380 0.2523 8 -1 -1 5 0 heads_default wzcagcccdccqcccccccccycccccccdccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1295,1356,1417,1478 215,182,248,281 0,0,0,0" << std::endl;
          *fs << "600 div:int (none) 209 1 1 99 96 380 0.252253 8 -1 -1 6 0 heads_default wzcancccdccqcccccccccycccccccdccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1236 215 0" << std::endl;
          *fs << "554 div:int (none) 454 1 1 101 97 388 0.251948 0 -1 -1 6 0 heads_default wzcagcocccccccccccccccccccsccckcccccccpccccccccccccccccccccfcsccccccccccucccccccccccccqccccczvfctxgab 1234 277 0" << std::endl;
          *fs << "94 div:int (none) 41 5 10 100 97 385 0.251744 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccscccccccccccpcccccccccccccccccccccsccccccccccucccccccccccccqccccczvfcaxgab 1228,1289,1352,1411,1472 330,362,263,296,296 0,0,0,0,0" << std::endl;
          *fs << "258 div:int (none) 131 1 1 98 95 379 0.25022 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccccccccocccccccccccccccccccccccccccccccccccmcccccccccccccccccivfcaxgab 2246 1419 0" << std::endl;
          *fs << "120 div:int (none) 77 1 5 98 86 369 0.232826 0 -1 -1 5 0 heads_default wzcagcbccccccccccccccccccccccccccccccccccccccccccccocccccccccccccccccccmczccccccccccccccczvfcaxgab 2183 131 0" << std::endl;
          *fs << "557 div:int (none) 26 1 1 100 97 388 0.25 0 -1 -1 3 0 heads_default wzcagccccccccccccjccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2136 280 0" << std::endl;
          *fs << "143 div:int (none) 101 1 3 100 97 385 0.250646 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccciccccccccccccccccccccccccccccccccccccccccccccccccgccccccpcicccccczvfcaxgab 1956 263 0" << std::endl;
          *fs << "396 div:int (none) 213 1 1 99 86 372 0.230563 0 -1 -1 4 0 heads_default wzcagccccccfcccccccccccccccccyccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab 1947 48 0" << std::endl;
          *fs << "626 div:int (none) 309 1 1 98 94 376 0.249779 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccccccacrccccccccccccccccccccccyccccnccccccoccccccccccccccacccccecccczvfcaxgab 1879 113 0" << std::endl;
          *fs << "465 div:int (none) 20 1 2 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccceccccccccccccsccccccccccccccccccccccccccccccccocccccccccccccczvfcaxgab 1835 264 0" << std::endl;
          *fs << "281 div:int (none) 15 1 1 99 96 384 0.249907 0 -1 -1 3 0 heads_default wzcagjccccccccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab 1765 1452 0" << std::endl;
          *fs << "603 div:int (none) 273 1 1 100 88 376 0.233424 0 -1 -1 3 0 heads_default wzcagcccccccpcccccccccccpccccccccccccccccccccccccccctccccccscccccccccccccczccccccccccccmccczvfcaxgab 1711 198 0" << std::endl;
          *fs << "580 div:int (none) 20 1 1 100 97 387 0.250607 0 -1 -1 3 0 heads_default wzcagccccccccccccrcccccjcccccceccccccccccccsccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 1659 231 0" << std::endl;
          *fs << "442 div:int (none) 60 2 2 99 96 381 0.251948 0 -1 -1 4 0 heads_default wzcagcccdccccccccccccmcccccccccccccccccccpccccccccccccccccccccccccpccccccccccccccccccccccczvfcaxgab 1476,1536 263,264 0,0" << std::endl;
          *fs << "189 div:int (none) 152 1 1 100 49 335 0.251295 0 -1 -1 5 0 heads_default ccccccccccsccccccccccucccccccccccccqccccczvhcaxgabwzcagcccccccccccccccccccccccccccccccccpccccccccccc 1347 893 0" << std::endl;
          *fs << "511 div:int (none) 317 1 1 98 91 373 0.25 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccccccfcccmctcccicccccccccccccucccccccccccccccccccccccccccehccccczvfcaxgab 1344 32 0" << std::endl;
          *fs << "488 div:int (none) 103 2 2 99 96 380 0.251969 0 -1 -1 5 0 heads_default wzcagcccdccccccccccccccccccccdccccccccccccccccccccccccgcccccccccccccccccccccccycccccccccpczvfcaxgab 1296,1355 237,204 0,0" << std::endl;
          *fs << "419 div:int (none) 340 1 1 100 49 332 0.252604 0 -1 -1 7 0 heads_default cccccccccccccccccccccccpcccncdccccncccccczvlcaxgabwzcagccccccccscccccccccccccccccdccccccccccbccccccc 1230 356 0" << std::endl;
          *fs << "595 div:int (none) 210 1 1 100 97 387 0.250369 0 -1 -1 4 0 heads_default wzcagcccccccccccccccyccccccccccscccccccccccccccccccccccccccccccccccccccccccccccccccckcccccnzvfcaxgab 2260 248 0" << std::endl;
          *fs << "296 div:int (none) 217 2 2 98 95 375 0.251328 0 -1 -1 7 0 heads_default wzcagcctccccccrccccccccccccccpccccgccccaccccaccccccccclcccccccccccccccccccccccccccucccccczvfcaxgab 2196,2257 164,230 0,0" << std::endl;
          *fs << "181 div:int (none) 4 1 3 98 95 379 0.250331 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccccciccccccccccccccccccccscccccccccccccccccccccccccczvfcaxgab 1887 296 0" << std::endl;
          *fs << "572 div:int (none) 287 1 1 98 96 382 0.251298 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccpccccccccccccccccccccccccccccccccccccccccccccccccczclyccccccccccccczvfaxgab 1831 269 0" << std::endl;
          *fs << "526 div:int (none) 413 1 1 98 95 376 0.25 8 -1 -1 8 0 heads_default wzcagcccccccccccccccccceccccrcccccccctccccicccccccccccccncccccccccbcccccccccccacccccecccczvfcaxgab 1819 363 0" << std::endl;
          *fs << "480 div:int (none) 372 2 2 100 97 385 0.251295 0 -1 -1 4 0 heads_default wzcagcccccccccccccccctcccccccceccccccccccccsccccccccccccccccpcccccccccccccccccccccccccccccczvfcaxgab 1721,1782 230,198 0,0" << std::endl;
          *fs << "273 div:int (none) 80 2 3 100 88 376 0.2333 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccpccccccccccccccccccccccccccctccccccscccccccccccccczcccccccccccccccczvfcaxgab 1712,1713 230,197 0,0" << std::endl;
          *fs << "618 div:int (none) 422 1 1 98 93 570 0.25066 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccccrzccccccccccccccccccccccccccccccccccccccccccccccccacccchrcccczvfcaxgab 1702 170 0" << std::endl;
          *fs << "20 div:int (none) 12 7 21 100 97 387 0.250609 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccccccceccccccccccccsccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 1658,1718,1777,1836,1899,1900,1961 230,230,230,296,296,263,264 0,0,0,0,0,0,0" << std::endl;
          *fs << "319 div:int (none) 42 1 1 100 97 386 0.250646 0 -1 -1 2 0 heads_default wzcagccccccccscccccccccccccccccccccccccccccccccchccccccccccccccccccccccccccccnccccccccccccczvfcaxgab 1589 362 0" << std::endl;
          *fs << "342 div:int (none) 8 2 2 98 95 379 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccccmccacccccccccczvfcaxgab 1584,1645 197,165 0,0" << std::endl;
          *fs << "457 div:int (none) 185 1 2 101 98 388 0.250639 0 -1 -1 3 0 heads_default wzcagccccccccscccccccccccccvcccccccccccccccccccccccccccccccccccccccqccccccccccnccccccccccccmzvfcaxgab 1531 264 0" << std::endl;
          *fs << "365 div:int (none) 41 1 2 100 97 385 0.251295 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccsccccccccdcucccccccccccccqccccczvfcaxgab 1529 197 0" << std::endl;
          *fs << "434 div:int (none) 194 2 2 100 97 384 0.251948 0 -1 -1 5 0 heads_default wzcagccccccccsccccccccccccccccccccccccccccbccccccccccccccccccpcccccccccccccccncdccccncacccczvfcaxgab 1235,1294 363,383 0,0" << std::endl;
          *fs << "420 div:int (none) 253 1 1 99 96 381 0.251309 0 -1 -1 7 0 heads_default wzcagcccccwccccccccccjcccccccccccccccjcccccccccccccccccccccccccccccxcccccccccccccccgccccuczvfcaxgab 2256 722 0" << std::endl;
          *fs << "144 div:int (none) 95 6 8 98 95 377 0.251989 0 -1 -1 6 0 heads_default wzcagcccccccccccqcccjccccccccccccccccccccccccyccccccccccccccccccmccccccccccccccccdmcccccczvfcaxgab 2189,2248,2308,2309,2368,2369 239,272,305,272,338,206 0,0,0,0,0,0" << std::endl;
          *fs << "581 div:int (none) 144 1 1 98 95 377 0.251989 0 -1 -1 7 0 heads_default wzcagcccccccccccqcccjccccccccccccccccccccccccyccccccccccccccccccmccccccccccccccccdmcccccczvfcaxaab 2188 239 0" << std::endl;
          *fs << "604 div:int (none) 494 1 1 99 96 383 0.25 0 -1 -1 5 0 heads_default wzcagccccccccucccccfyccccccccccccccccccccccccccccccccaccccccccccccccccccccccccccncccccxccnzvfcaxgab 2074 212 0" << std::endl;
          *fs << "259 div:int (none) 8 1 1 98 95 380 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrcccccceccccccccccccccccccccccccccccccccccccccccccacccccccccczvfcaxgxb 2003 1419 0" << std::endl;
          *fs << "627 div:int (none) 398 1 1 99 92 380 0.240209 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccecccccccccccccccccccccfccccncccccccccccccccccccccccccccczvfcaxgab 1830 66 0" << std::endl;
          *fs << "512 div:int (none) 326 1 1 98 94 376 0.25 0 -1 -1 7 0 heads_default wzcagccccccccccccccccccecccprcccccccccccccccccccccdcccccncccccccccbcccccccccccacccccecccczvfcaxgab 1761 363 0" << std::endl;
          *fs << "489 div:int (none) 83 1 1 101 48 336 0.250646 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccepcccccccccccccccccccccccccccmccccccccccccccccccccccccccccccczvfciaxgab 1597 131 0" << std::endl;
          *fs << "75 div:int (none) 23 1 7 98 94 378 0.249339 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccccicccccccccccccccccccccccccdcccccccccccccccccccccczvfcaxgab 1586 264 0" << std::endl;
          *fs << "558 div:int (none) 209 1 1 99 96 380 0.252234 8 -1 -1 6 0 heads_default wzcagcccdccqcccccccccycccccccdccccccccccccccrcccmcccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1419 281 0" << std::endl;
          *fs << "374 div:int (none) 103 1 1 99 96 381 0.251776 0 -1 -1 5 0 heads_default wncagcccdccccccccccccccccccccdccccccicccccccccccccccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1357 974 0" << std::endl;
          *fs << "535 div:int (none) 446 1 1 99 96 379 0.252632 0 -1 -1 8 0 heads_default wzcagcccdccqccccccccwycccccccdccccccccccdccccccccsccccgccccccccccccccccccccccccxscccccccpczvfcaxgab 1238 301 0" << std::endl;
          *fs << "243 div:int (none) 36 2 4 98 95 377 0.251326 0 -1 -1 4 0 heads_default wzcagccccccccccxccccccccccccccccccvccccccccccccccccccccccccccccccccccccmccccccccccccccccczvfcaxgab 2243,2244 45,375 0,0" << std::endl;
          *fs << "588 div:int (none) 197 1 1 98 91 373 0.243859 0 -1 -1 7 0 heads_default wzcagcbcccccccccccccccccccccrccyccccccccccjcfcccczccbcccccccccccccccccccccccccacccccccccczvfcaxgab 2121 258 0" << std::endl;
          *fs << "197 div:int (none) 136 3 6 98 91 373 0.243875 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccccrccyccccccccccjcfcccczccbcccccccccccccccccccccccccacccccccccczvfcaxgab 2001,2061,2182 225,258,225 0,0,0" << std::endl;
          *fs << "611 div:int (none) 103 1 1 99 96 381 0.251857 0 -1 -1 5 0 heads_default wzcagcccdccccccccccccccccccccdccccccccccccccccccccccccgcccccccccccccccccccccccccclccccccpczvfcaxgab 1479 231 0" << std::endl;
          *fs << "565 div:int (none) 94 1 1 100 97 385 0.25173 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccscccccccccccpzccccccccccccccccccccsccccccccccucccccccccccccqccccczvfcaxgab 1471 264 0" << std::endl;
          *fs << "266 div:int (none) 41 1 2 100 97 385 0.251295 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccucccccccccccccgccccczvfcaxgab 1467 296 0" << std::endl;
          *fs << "542 div:int (none) 148 1 1 100 97 384 0.25251 0 -1 -1 6 0 heads_default wzcagccccccccqcccccccccccccccccqccgcccpcccccccccccccccccccccsccccccccccuccaccccccccdcqccccczvfcaxgab 1286 330 0" << std::endl;
          *fs << "634 div:int (none) 511 1 1 98 91 373 0.25 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccccccccccfcmcpctcccicccccccccccccucccccccccccccccccccccccccccehccccczvfcaxgab 1283 33 0" << std::endl;
          *fs << "539 div:int (none) 359 1 1 99 96 379 0.251309 0 -1 -1 5 0 heads_default wzcagccdcccccccccccccccccccccccccmcckcccccccccccvccccccccccccccccccccccccccccccccccucccccczvfcaxgab 2128 330 0" << std::endl;
          *fs << "585 div:int (none) 389 1 1 100 97 386 0.250646 0 -1 -1 5 0 heads_default wzcagcccccccccccccccyccccccccccscccccccccccccccccccccccccccccccccckccccccccccccccccoccccccnzvfcaxgab 2077 248 0" << std::endl;
          *fs << "56 div:int (none) 4 2 6 98 95 380 0.249708 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccciccccccccccccccczvfcaxgab 2008,2067 363,362 0,0" << std::endl;
          *fs << "263 div:int (none) 39 1 1 100 96 382 0.251028 0 -1 -1 4 0 heads_default wzcagccdcccccccccccccccccccccccccmcccccccccccccccuccccccccccccccccccccccccccccccccnuccccfcczvfcaxgab 1950 1485 0" << std::endl;
          *fs << "309 div:int (none) 168 3 4 98 94 376 0.249834 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccacrcccccccccccccccccccccccccccnccccccoccccccccccccccacccccecccczvfcaxgab 1880,1881,1882 98,65,66 0,0,0" << std::endl;
          *fs << "493 div:int (none) 273 1 1 100 88 376 0.234043 8 -1 -1 3 0 heads_default wzcagcccccccccccccccccccpccccccccccccccccccccccccccctpcccccscccccccccccccczcccccccccccccccczvfcaxgab 1654 131 0" << std::endl;
          *fs << "631 div:int (none) 308 1 1 98 95 377 0.251323 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccccpcccrcccccclcccccccccccccccccccccccccccccccccccccccdccacccccrcccczvfcaxgab 1641 66 0" << std::endl;
          *fs << "470 div:int (none) 342 1 1 98 95 379 0.25 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccccmccaccccccccccrvfcaxgab 1524 660 0" << std::endl;
          *fs << "194 div:int (none) 133 2 3 100 97 385 0.251556 0 -1 -1 4 0 heads_default wzcagccccccccsccccccccccccccccccccccccccccbccccccccccccccccccccccccccccccccccncdccccncccccczvfcaxgab 1353,1354 296,297 0,0" << std::endl;
          *fs << "562 div:int (none) 344 1 1 100 97 384 0.251731 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccvcccccccccccpcccccccccccccccccccccscccccccdccucccccccccccccqccclczvfcaxgab 1349 264 0" << std::endl;
          *fs << "608 div:int (none) 483 1 1 98 95 379 0.25066 0 -1 -1 7 0 heads_default wzcagccccccccccccccccccccqctrccccccpcccccccqcccccccccccccqccdcccccccccccccccccacccccccccczvfcaxgab 1342 198 0" << std::endl;
          *fs << "148 div:int (none) 88 2 5 100 97 384 0.252522 0 -1 -1 5 0 heads_default wzcagccccccccqccccccccccccccccccccgcccpcccccccccccccccccccccsccccccccccuccaccccccccdcqccccczvfcaxgab 1287,1346 296,296 0,0" << std::endl;
          *fs << "538 div:int (none) 254 1 1 99 95 378 0.251323 0 -1 -1 7 0 heads_default wzcagcccucccccccccccjcccccyccccccccccccccxcccyccccccccccccccccccccccccbccccccccccdemcccccczvfcaxgab 2430 303 0" << std::endl;
          *fs << "584 div:int (none) 144 1 1 98 95 377 0.251989 0 -1 -1 7 0 heads_default wzcagcccccccccccqnccjccccccccccccccccccccccccyccccccccccccccccccmccccccccccccccccdmcccccczvfcaxgab 2429 238 0" << std::endl;
          *fs << "492 div:int (none) 120 2 2 98 86 370 0.233062 0 -1 -1 6 0 heads_default wzcagcbccccccccccccccbcccccccccccccccccccccccccccccocccccccccccccccccccwczccccccccccccccczvfcaxgab 2242,2301 164,198 0,0" << std::endl;
          *fs << "607 div:int (none) 26 1 1 100 97 388 0.25 0 -1 -1 3 0 heads_default wzcagcccccccccccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnavfcaxgab 2134 215 0" << std::endl;
          *fs << "377 div:int (none) 136 1 1 98 82 364 0.243316 0 -1 -1 6 0 heads_default wzcagcccccccczccccccccccccccrccyccccccccccccfcccczccccccccccccccccccccccccccccacccccccccczvfcaxgab 2005 149 0" << std::endl;
          *fs << "515 div:int (none) 246 1 1 97 95 377 0.251989 0 -1 -1 6 0 heads_default wzcagccccccccccqccccccccccccrccccccccccccccccccccccccccccccccccvcccccccccccccccccccccccczvfcaxgab 1942 363 0" << std::endl;
          *fs << "423 div:int (none) 46 1 1 98 94 377 0.24942 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccncccccccccccccccccccccacccccecccczvfcaxzab 1886 660 0" << std::endl;
          *fs << "630 div:int (none) 523 1 1 98 48 278 0.249337 8 -1 -1 6 0 heads_default cccccccncccccccccccccccccccccacccccecccczvfcjxgabwzcagccccwccccccccccccccccccrcccccccccccccccccccc 1883 45 0" << std::endl;
          *fs << "400 div:int (none) 168 3 4 98 94 374 0.250004 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccacrccccccccccvccccccccccccccccncccccccccccccccccccccacccccecccczvfcaxgab 1763,1764,1823 99,98,98 0,0,0" << std::endl;
          *fs << "124 div:int (none) 8 1 4 98 95 379 0.250189 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccsccccacccccccccczvfcaxgab 1644 296 0" << std::endl;
          *fs << "469 div:int (none) 379 1 1 100 97 386 0.251295 0 -1 -1 4 0 heads_default wzcagcccccccccccwcccccccccccccecccccccccccccccccccccccccccccccccccsccccccccccccccccccactccczvfcaxgab 1599 230 0" << std::endl;
          *fs << "561 div:int (none) 83 1 1 99 97 387 0.250517 0 -1 -1 3 0 heads_default wcagcccccccccccccccccccccccccecccccccccccwccccccccccccccccmccccccccccccccccccccccccccccccczvfcaxgab 1480 297 0" << std::endl;
          *fs << "285 div:int (none) 209 1 1 99 96 380 0.252632 0 -1 -1 6 0 heads_default wzcagcccdccqccccccccwycccccccdccccccccccccccccccccccccgccccccccccccccccccccccccxccccccccpczvfcaxgab 1358 336 0" << std::endl;
          *fs << "446 div:int (none) 285 1 1 99 96 379 0.252632 0 -1 -1 7 0 heads_default wzcagcccdccqccccccccwycccccccdcccccccccccccccccccsccccgccccccccccccccccccccccccxccccccccpczvfcaxgab 1297 302 0" << std::endl;
          *fs << "170 div:int (none) 141 1 1 100 49 284 0.251948 0 -1 -1 6 0 heads_default ccccccccccsccccccccccucccccccccccccqccccczvfcyxgabwzcagcccccccccccccccccccccsccccccccccceccccccccccc 1290 960 0" << std::endl;
          *fs << "337 div:int (none) 39 1 1 99 96 381 0.251309 0 -1 -1 4 0 heads_default wzcagccdcccccccccccccccccccccccccmcccccccccccccccccccccdcccccccccccccccccccccccccccucccccczvfcaxgab 2313 329 0" << std::endl;
          *fs << "291 div:int (none) 120 1 1 98 86 369 0.232642 0 -1 -1 6 0 heads_default wzcagcbccccccccccccccccccccccccccccccccccccccccccccocccccccccccccccccccmczccccccccccccccczvzcaxgab 2245 1266 0" << std::endl;
          *fs << "360 div:int (none) 217 4 4 98 95 377 0.251103 0 -1 -1 7 0 heads_default wzcagccccccccccccccccccccccccpccccwccccaccccacccccccccccmcccccccccccccccccccccccccucjcccczvfcaxgab 2194,2253,2254,2255 230,231,296,297 0,0,0,0" << std::endl;
          *fs << "521 div:int (none) 131 1 1 98 95 379 0.25044 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccccccccocccccccccccccccccccccccccccccccccccmcccccccccccccbccczvfcaxgab 2187 330 0" << std::endl;
          *fs << "429 div:int (none) 36 1 2 98 95 380 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccimccccccccccccccccczvfcaxgab 2185 330 0" << std::endl;
          *fs << "475 div:int (none) 284 2 2 98 86 368 0.233696 0 -1 -1 7 0 heads_default wzcagcccqcccccccccccccccccccrccycccccccccccccccccccccccccccacccccccccccccjccczacccccccccczvfcaxgab 2181,2240 192,192 0,0" << std::endl;
          *fs << "61 div:int (none) 10 1 4 99 96 383 0.250082 0 -1 -1 3 0 heads_default wzcagccccccccccccccccjcccccccccccccccccccccccccccccccccccccccccccccxcccccccccccccccucccccczvfcaxgab 1952 294 0" << std::endl;
          *fs << "613 div:int (none) 162 1 1 99 96 382 0.25 0 -1 -1 5 0 heads_default wzcagcccccccccccccccycccccccyccclccccccccccccccccccccccccccccccccccccccccccccccccclccccccnzvfcaxgab 1838 189 0" << std::endl;
          *fs << "314 div:int (none) 114 1 1 99 96 383 0.250653 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccckrcccccccccccccccwcccccccccccccccccccccbcclcccccccccccccccccccczvfcaxgab 1766 32 0" << std::endl;
          *fs << "590 div:int (none) 293 1 1 100 97 382 0.253529 0 -1 -1 6 0 heads_default wzcagcccdccccccccclccccccccccccccccccccccccccccbcgccccgcccccccccccccccccccccpccccccccccccvczvfcaxgab 1715 231 0" << std::endl;
          *fs << "222 div:int (none) 111 1 3 100 97 388 0.249357 0 -1 -1 3 0 heads_default wzcagcccccbcccccccccwcccccccxcccccccccccccccactcccccccccccccccccbcccccccccccccccccccccccccczvfcaxgab 1533 223 0" << std::endl;
          *fs << "544 div:int (none) 73 1 1 98 95 379 0.250566 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccjcccctrccccccpccccccccccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab 1343 297 0" << std::endl;
          *fs << "452 div:int (none) 262 1 1 100 97 383 0.252604 0 -1 -1 6 0 heads_default wzcagccccccccscccccccccccccccccdccccccccccbcccccccccccccccccccccchcccccccccccncdccccncccccczvfcaxgab 1291 296 0" << std::endl;
          *fs << "569 div:int (none) 371 1 1 99 96 383 0.250218 0 -1 -1 4 0 heads_default wzcagcccccyccccccccycccccccccccccvcccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2259 246 0" << std::endl;
          *fs << "615 div:int (none) 296 1 1 98 95 375 0.251328 0 -1 -1 8 0 heads_default wzcagcctccccccrccccccccccccccpccccgccccaccccaccccccccclcccccccccccccccceccccccccccucccccwzvfcaxgab 2197 198 0" << std::endl;
          *fs << "546 div:int (none) 432 1 1 100 97 383 0.252604 0 -1 -1 5 0 heads_default wzcagccccccccncccccccccccciccccccccccccccdbcccccccccccccaccccccccccccccccccgccccccpcicccccczvfcaxgab 2080 297 0" << std::endl;
          *fs << "63 div:int (none) 10 1 5 99 96 384 0.25 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccacccccccccccccccccccccccccccccccccccccucccccczvfcaxgab 1954 231 0" << std::endl;
          *fs << "500 div:int (none) 229 1 1 99 83 373 0.228249 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccczcccccccccccccccccccccccccccgcfccccccccccccccccccccccccccccccccczvfcaxgab 1948 362 0" << std::endl;
          *fs << "523 div:int (none) 46 1 1 98 48 278 0.249337 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccncccccccccccccccccccccacccccecccczvfcjxgab 1884 32 0" << std::endl;
          *fs << "431 div:int (none) 181 1 1 98 95 379 0.250221 0 -1 -1 4 0 heads_default wzcagccccgccccccccccccccccccccccccccccccciccccccccccccccccccccscccccccccccccccccccccccccczvfcaxgak 1826 693 0" << std::endl;
          *fs << "477 div:int (none) 392 1 1 100 96 383 0.250653 0 -1 -1 6 0 heads_default wzcagcccccqcccccccccycccccccccccccccccccacccccoccccccccccecacccclccccccccdcccccccccccccccccztfcaxgab 1710 644 0" << std::endl;
          *fs << "362 div:int (none) 75 1 1 98 94 378 0.249735 0 -1 -1 4 0 heads_default whcagcccccccccccccccccccccccccccccccccccicccccccccccccccccccccccpcdcccccccccccccccccccccczvfcaxgab 1707 1005 0" << std::endl;
          *fs << "293 div:int (none) 228 2 4 100 97 382 0.253529 0 -1 -1 5 0 heads_default wzcagcccdccccccccccccccccccccccccccccccccccccccbcgccccgcccccccccccccccccccccpccccccccccccvczvfcaxgab 1655,1657 230,264 0,0" << std::endl;
          *fs << "316 div:int (none) 236 1 1 101 98 388 0.251969 0 -1 -1 6 0 heads_default aawzcagcccccccccccccccccchcccctccccccrccccccccckcxcccccccccccccscccbccccccccaccccccccccccccczvfcaxgab 1593 372 0" << std::endl;
          *fs << "408 div:int (none) 177 1 1 98 95 375 0.251989 0 -1 -1 5 0 heads_default wzcagcdcccccccccccccccccecccccwccccctcccicccccccccccccucccccccccccccccccccccccccccecccccczvfcaxgab 1464 362 0" << std::endl;
          *fs << "454 div:int (none) 94 1 1 101 97 388 0.251948 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccscccccccccccpccccccccccccccccccccfcsccccccccccucccccccccccccqccccczvfcaxgab 1293 263 0" << std::endl;
          *fs << "333 div:int (none) 253 3 4 99 96 380 0.25219 0 -1 -1 7 0 heads_default wzcagcccccwcuccccccccjcccccccccccccccjcccccccccccccccccccacccccccccxcccccccccccccccuccccuczvfcaxgab 2315,2316,2376 358,292,292 0,0,0" << std::endl;
          *fs << "632 div:int (none) 243 1 1 98 95 377 0.251326 0 -1 -1 5 0 heads_default wzcagccccccccccxccccccccccccccccccvccccccctccccccccccccccccccccccccccccmccccccccccccccccczvfcaxgab 2304 45 0" << std::endl;
          *fs << "586 div:int (none) 36 1 1 98 95 380 0.249836 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccmcccccccccccccqccczvfcaxgab 2066 231 0" << std::endl;
          *fs << "494 div:int (none) 67 1 1 99 96 383 0.25 0 -1 -1 4 0 heads_default wzcagccccccccuccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccxccnzvfcaxgab 2015 180 0" << std::endl;
          *fs << "287 div:int (none) 183 1 3 99 96 382 0.251298 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccpccccccccccccccccccccccccccccccccccccccccccccccccczclyccccccccccccczvfcaxgab 1891 269 0" << std::endl;
          *fs << "563 div:int (none) 227 1 1 99 95 378 0.250793 0 -1 -1 6 0 heads_default wzcagcccrchcccccccccccccccccccwccccccccccicciccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 1889 264 0" << std::endl;
          *fs << "471 div:int (none) 297 2 2 98 95 377 0.251323 0 -1 -1 7 0 heads_default wzcagcccccccccccccccyoccccccyccccccqcccccccccccccccccccccccccoccccccccccccccccccclccccccnzvfcaxgab 1840,1841 288,288 0,0" << std::endl;
          *fs << "609 div:int (none) 273 1 1 99 88 376 0.2333 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccpcccmcccccccccccccccccccccctccccccscccccccccccccczcccccccccccccccczvfcaggab 1772 198 0" << std::endl;
          *fs << "517 div:int (none) 320 1 1 198 95 380 0.249337 0 -1 -1 7 0 heads_default aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaawzcagcccccccccccccccccccccccrccccchccccccccccccccccccccccncccccccccvccccqccccccacccccecccczvfcaxgab 1699 330 0" << std::endl;
          *fs << "402 div:int (none) 232 4 4 98 95 377 0.250221 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccciccacrccccbccccccccccccccccccccccncccccccccccccccccccccacxcccecccczvfcaxgab 1698,1700,1701,1759 33,65,66,32 0,0,0,0" << std::endl;
          *fs << "103 div:int (none) 37 3 7 99 96 381 0.251857 0 -1 -1 4 0 heads_default wzcagcccdccccccccccccccccccccdccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1418,1477,1538 197,230,231 0,0,0" << std::endl;
          *fs << "540 div:int (none) 285 1 1 99 96 380 0.252632 0 -1 -1 7 0 heads_default wzcagcccdpcqccccccccwycccccccdccccccccccccccccccccccccgccccccccccccccccccccccccxccccccccpczvfcaxgab 1299 303 0" << std::endl;
          *fs << "341 div:int (none) 84 3 4 98 95 378 0.250881 0 -1 -1 5 0 heads_default wzcagcccccccciccccccccccccccccccccccccccccbccyccccccccccccccccccccccccccccccccccccpcccccczvfcaxgab 2250,2311,2371 305,305,305 0,0,0" << std::endl;
          *fs << "617 div:int (none) 393 1 1 99 94 379 0.248021 0 -1 -1 7 0 heads_default wzcagcccccccccccccccycccccccccccccczcccccccccccccccccccccccccccccccclcccccccccccicccczcccnzvfcaxgab 2140 182 0" << std::endl;
          *fs << "594 div:int (none) 210 1 1 99 97 387 0.250323 0 -1 -1 4 0 heads_default wpcagccccacccccccccyccccccccccsccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2078 215 0" << std::endl;
          *fs << "203 div:int (none) 12 2 2 100 97 387 0.25 0 -1 -1 2 0 heads_default wzcagcccccccccccwcccccccccccccecccccccccccccccccccccccccccccccccccscccccccccccccccccccccccczvfcaxgab 1716,1717 198,164 0,0" << std::endl;
          *fs << "571 div:int (none) 257 1 1 98 95 379 0.25066 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccctrccccccpcccccccqccccccccccccccccccccccccccccccccdcacccccccccczvfcaxgab 1523 264 0" << std::endl;
          *fs << "180 div:int (none) 124 2 4 98 95 379 0.25066 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccjccccccccccccccccccccccccccccccccccccccccccccsccccacccccccccczvfcaxgab 1466,1525 330,362 0,0" << std::endl;
          *fs << "387 div:int (none) 150 1 2 104 99 394 0.251295 0 -1 -1 5 0 heads_default aaaawzccgcccdcccccccccccccccdcccccccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccccczvfcaxgab 1415 990 0" << std::endl;
          *fs << "548 div:int (none) 447 1 1 100 97 384 0.251948 0 -1 -1 6 0 heads_default wzcagccccctcccccccccccccccscccccccccccpcccccccccccccccccccccsccccckccccucccccccccccccqccccczvfcaxgab 1410 297 0" << std::endl;
          *fs << "456 div:int (none) 94 1 1 100 97 381 0.251948 0 -1 -1 5 0 heads_default wzcagcccccjcccccccccccccccscccccccccvcpcccccccccccccccccccccsccccccccccucccccccccccccqccccczvfcaxgab 1350 296 0" << std::endl;
          *fs << "410 div:int (none) 238 1 2 98 95 375 0.25266 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccclccwccccctcccicccccccucccccucccccccccccccccccccccccccccecccccczvfcaxgab 1345 363 0" << std::endl;
          *fs << "294 div:int (none) 144 1 2 98 95 376 0.252213 0 -1 -1 7 0 heads_default wzcagcccccccccccqcccjccccccccccccccccccccccccyccccccccqcccccccccmcqccccccccccckccdmcccccczvfcaxgab 2310 206 0" << std::endl;
          *fs << "455 div:int (none) 144 1 1 98 95 377 0.251989 0 -1 -1 7 0 heads_default wzcqgcccccccccccqcccjccccccccccccccccccccccccnccccccccccccccccccmccccccccccccccccdmcccccczvfcaxgab 2307 627 0" << std::endl;
          *fs << "409 div:int (none) 243 1 1 98 95 377 0.251989 0 -1 -1 5 0 heads_default wzcagccccccccccxcccccccccccccbccccvccccccccccccccccccccccccccccccccccccmccccccccccccccccczvfcaxgab 2305 375 0" << std::endl;
          *fs << "179 div:int (none) 54 3 5 98 95 378 0.251323 0 -1 -1 5 0 heads_default wzcagcccccccccccccccjccccccccccccccccccccccccyccccccccccccccccccccccccbccccccccccdmcccccczvfcaxgab 2190,2251,2252 305,371,371 0,0,0" << std::endl;
          *fs << "432 div:int (none) 272 1 1 100 97 383 0.252604 0 -1 -1 4 0 heads_default wzcagccccccccncccccccccccciccccccccccccccdbccccccccccccccccccccccccccccccccgccccccpcicccccczvfcaxgab 2139 296 0" << std::endl;
          *fs << "593 div:int (none) 298 1 1 98 96 382 0.251309 0 -1 -1 6 0 heads_default wzcagcccccccccccccccycccccccycccccccccccccccccccccccccccccccccccccccccccccccccccxclcccccnzvfcaxgab 1959 207 0" << std::endl;
          *fs << "524 div:int (none) 428 1 1 99 96 379 0.251989 0 -1 -1 7 0 heads_default wzcagccccccccccqccccccccccccrcccccccccccccccccdcccccccccccccccccvccccccbccccccrafccccccccxzvfcaxgab 1940 330 0" << std::endl;
          *fs << "616 div:int (none) 67 1 1 99 96 384 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccyccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 1895 182 0" << std::endl;
          *fs << "478 div:int (none) 203 1 2 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzcagccccccclcccwcccccccccccccecccccccccccccccccccccccccccccccccccscccccccccccccccccccccccczvfcaxgab 1776 198 0" << std::endl;
          *fs << "570 div:int (none) 269 1 1 100 97 386 0.251035 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccoccicccceccccccccccccccccciccccccccccuccccccccccccccccccccccctccccccczvfcaxgab 1600 264 0" << std::endl;
          *fs << "547 div:int (none) 60 1 1 100 97 385 0.251803 0 -1 -1 4 0 heads_default wzcagcccdcccccccccccccmccccccccccccccccccciccccccccccccccccccccccccpccccccccccccccccccccccczvfcaxgab 1473 297 0" << std::endl;
          *fs << "41 div:int (none) 21 1 6 100 97 386 0.250925 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccucccccccccccccqccccczvfcaxgab 1469 362 0" << std::endl;
          *fs << "248 div:int (none) 180 1 3 98 95 378 0.25066 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccecccccjccccccccccccccccccccccccccccccccccccccccccccsccccacccccccccczvfcaxgab 1461 363 0" << std::endl;
          *fs << "317 div:int (none) 177 1 1 98 94 376 0.251989 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccccwcccmctcccicccccccccccccucccccccccccccccccccccccccccehccccczvfcaxgab 1404 362 0" << std::endl;
          *fs << "363 div:int (none) 285 1 2 99 96 380 0.252632 0 -1 -1 7 0 heads_default wzcagcccdccqccccccccwycccccccdccccccccccccccccccccccccgcccccccccccccccaccccccccxccccccccpczvfcaxgab 1359 270 0" << std::endl;
          *fs << "506 div:int (none) 409 1 1 98 95 377 0.251989 0 -1 -1 6 0 heads_default wzcagccccccocccxcccccccccfcccbccccvccccccccccccccccccccccccccccccccccccmccccccccccccccccczvfcaxgab 2364 369 0" << std::endl;
          *fs << "621 div:int (none) 120 1 1 98 86 369 0.232826 0 -1 -1 6 0 heads_default wzcagcbvcccccccccccccccccccccccccccccccccccccccccccocccccccccccccccccccmczccccccccccccccczvfcaxgab 2184 132 0" << std::endl;
          *fs << "184 div:int (none) 132 1 3 99 96 382 0.250817 0 -1 -1 5 0 heads_default wzcagcccccwccccccccccjcccccccccccccccjcccccccccccccccccccccccccccccxcccccccccccccccucccccczvfcaxgab 2133 326 0" << std::endl;
          *fs << "529 div:int (none) 334 1 1 98 94 376 0.25 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccyccccccccccccccccccccctccccccccccccccccccccczvfcaxgab 2010 306 0" << std::endl;
          *fs << "437 div:int (none) 265 1 1 99 96 383 0.25066 0 -1 -1 6 0 heads_default wzcagccccccccccccuccccccccccrccqcccccccccccccccccccccccccccccccccccccccccccbcccacccccccccczvfcaxgab 1945 329 0" << std::endl;
          *fs << "552 div:int (none) 366 1 1 100 97 386 0.251309 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccpuccccccccycccccccccccccccccccccccccxcccccccccccccczclyccccccccccccczvfcaxgab 1893 302 0" << std::endl;
          *fs << "46 div:int (none) 8 1 4 98 94 377 0.249411 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccncccccccccccccccccccccacccccecccczvfcaxgab 1825 329 0" << std::endl;
          *fs << "460 div:int (none) 18 1 2 100 97 387 0.25 0 -1 -1 2 0 heads_default wzcagccccccccccccrcccccccccccccccccccccccccccccccccccccccccccsccccccccccccccccccccccccccccczvfcaxgab 1775 264 0" << std::endl;
          *fs << "322 div:int (none) 121 2 2 100 96 385 0.250646 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccicccccccwccccceccccccccccgcczvfcaxgab 1768,1769 384,362 0,0" << std::endl;
          *fs << "575 div:int (none) 297 1 1 98 95 378 0.251309 8 -1 -1 7 0 heads_default wzcagccccccccccccccchoccccccyccccccqcccccccccccccccccccccccccccccccccccfccccccccclccccccnzvfcaxgab 1719 254 0" << std::endl;
          *fs << "598 div:int (none) 365 1 1 100 97 385 0.251295 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccccccyccccpccccccccccccccccpccccsccccccccdcucccccccccccccqccccczvfcaxgab 1470 228 0" << std::endl;
          *fs << "483 div:int (none) 257 1 1 98 95 379 0.25066 0 -1 -1 6 0 heads_default wzcagccccccccccccccccccccqctrccccccpcccccccqccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab 1401 230 0" << std::endl;
          *fs << "441 div:int (none) 382 1 1 98 48 269 0.233062 0 -1 -1 7 0 heads_default ccocccccccccccccccccccmczccccccccccccccczvfcmxgabwzcagcbcccccccccccccccccaccxcccccccccccgceccccccc 2302 340 0" << std::endl;
          *fs << "602 div:int (none) 294 1 1 98 95 376 0.252213 0 -1 -1 8 0 heads_default wzcagcccccccccccqcccjccccccccccccccccccccccccyccccccccqcccccccccmcqcccccccccccwccdmcccccczvfcaxgab 2249 206 0" << std::endl;
          *fs << "4 div:int (none) 2 2 14 98 95 381 0.249345 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 2009,2070 296,297 0,0" << std::endl;
          *fs << "211 div:int (none) 145 1 1 98 95 379 0.25 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccclccccmccccccccccccicccczvfcsxlab 2006 1770 0" << std::endl;
          *fs << "533 div:int (none) 181 1 1 99 95 379 0.250331 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccccciccccccccccccccccccccscfccccccccccccccccccccccccczvfcaxgab 1828 330 0" << std::endl;
          *fs << "372 div:int (none) 20 1 1 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzcagcccccccccccccccctcccccccceccccccccccccsccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 1780 197 0" << std::endl;
          *fs << "579 div:int (none) 223 1 1 99 96 382 0.251309 0 -1 -1 6 0 heads_default wzclgcccccccccccccccyoccccccyccccccqcccccccccccccccccccccccccccccccccccccccccccccclccccccnzvfcaxgab 1778 223 0" << std::endl;
          *fs << "418 div:int (none) 322 1 1 100 96 384 0.249351 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccoccccccccccccccccicccccccwccccceccccccccccgcczvfcaxgab 1770 362 0" << std::endl;
          *fs << "487 div:int (none) 37 1 2 100 97 385 0.251295 0 -1 -1 4 0 heads_default wzcagcccdccccccccccccccccccccdccicccccccccccccccccccccgcccccccccccccccccccccccccccccccccccczvfcaxgab 1656 198 0" << std::endl;
          *fs << "165 div:int (none) 23 2 5 98 95 379 0.25011 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccccicccccccccccccccccccccccccccccccccycccccccccccccczvfcaxgab 1647,1708 235,202 0,0" << std::endl;
          *fs << "625 div:int (none) 504 1 1 99 48 278 0.252632 0 -1 -1 7 0 heads_default cccccccccccccccccccclccccccccccccccccgccczvfcxxgabwhcagcccccccccccccccccccccpcrcccccccccrcccccwcccc 1646 42 0" << std::endl;
          *fs << "510 div:int (none) 316 1 1 103 98 388 0.251969 0 -1 -1 7 0 heads_default aaaawzwagcccrcccccccccccccchcccctccccccrccccccccckcxcccccccccccccscccbccccccccaccccccccccccccczvfcaxgab 1532 375 0" << std::endl;
          *fs << "257 div:int (none) 73 2 3 98 95 379 0.25066 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccctrccccccpcccccccqccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab 1462,1463 230,230 0,0" << std::endl;
          *fs << "556 div:int (none) 266 1 1 100 97 385 0.251295 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccuccccccvccccccgccccczvfcaxgab 1406 264 0" << std::endl;
          *fs << "73 div:int (none) 8 1 6 98 95 379 0.250566 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccctrccccccpccccccccccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab 1402 296 0" << std::endl;
          *fs << "430 div:int (none) 39 2 2 99 86 371 0.251309 0 -1 -1 4 0 heads_default wzcagccdcccccccccccccccccccccccccmccccccccccccccccccccccccccccccccccccfccccccccccccucccccczvfcaxgab 2314,2375 296,297 0,0" << std::endl;
          *fs << "545 div:int (none) 371 1 1 99 96 383 0.25 0 -1 -1 4 0 heads_default wzcagcccccyccccccccycccccccccjcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2258 312 0" << std::endl;
          *fs << "315 div:int (none) 245 1 1 98 86 369 0.25066 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccbcczcccccccccccccccccccyfcccccccccccccccccccccccccccccccccccpcccccczvfcaxqab 2247 1018 0" << std::endl;
          *fs << "131 div:int (none) 36 1 4 98 95 379 0.25044 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccocccccccccccccccccccccccccccccccccccmccccccccccccccccczvfcaxgab 2186 362 0" << std::endl;
          *fs << "39 div:int (none) 10 5 19 99 96 382 0.251174 0 -1 -1 3 0 heads_default wzcagccdcccccccccccccccccccccccccmcccccccccccccccccccccccccccccccccccccccccccccccccucccccczvfcaxgab 2131,2132,2191,2192,2312 264,263,264,296,363 0,0,0,0,0" << std::endl;
          *fs << "568 div:int (none) 39 1 1 99 96 382 0.251174 0 -1 -1 4 0 heads_default wzcagccdccmccccccccccccccccccccccmcccccccccccccccccccccccccccccccccccccccccccccccccucccccczvfcaxgab 2073 330 0" << std::endl;
          *fs << "246 div:int (none) 174 2 3 98 95 377 0.251989 0 -1 -1 5 0 heads_default wzcagccccccccccqccccccccccccrcccccccccccccccccccccccccccccccccccvcccccccccccccacccccccccczvfcaxgab 1943,2002 329,329 0,0" << std::endl;
          *fs << "361 div:int (none) 88 1 1 100 97 384 0.251948 0 -1 -1 5 0 heads_default wzcagcccccnccqccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccucccccccccccdcqccccczvfcaxgab 1527 296 0" << std::endl;
          *fs << "499 div:int (none) 399 2 2 98 95 376 0.251989 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccrccccccccccccccccccccdcnacccccrcccczvfcaxgab 1459,1520 33,32 0,0" << std::endl;
          *fs << "614 div:int (none) 209 1 1 99 96 380 0.2523 0 -1 -1 6 0 heads_default wzcagcccdccqcccccccccycccccccdmcccccccccccccccccccccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1416 182 0" << std::endl;
          *fs << "292 div:int (none) 150 1 1 104 99 394 0.251295 0 -1 -1 5 0 heads_default aaaawzccgcccdcccccccccccccccdcccccccccccccccccccccccccccccgcccccccccccccccccecccccccccccccccccczvfcaxgab 1413 1386 0" << std::endl;
          *fs << "591 div:int (none) 257 1 1 97 95 379 0.25066 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccctrccccccpcccccccqcccccccccccccccccccccccccccccccccaccxcccuccczvfcaxgab 1403 240 0" << std::endl;
          *fs << "564 div:int (none) 61 1 1 100 96 383 0.250082 0 -1 -1 4 0 heads_default wzcagccccccccccccccccjccccccccccccccccccccccccccccccccccgcccccccccccxcccccccccccccccucccccczvfcaxgab 1951 261 0" << std::endl;
          *fs << "219 div:int (none) 63 1 3 99 96 383 0.250327 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccacccccccccccccccccccccccncccccccccccccucccccczvfcaxgab 1894 230 0" << std::endl;
          *fs << "265 div:int (none) 53 2 4 98 95 379 0.25066 0 -1 -1 5 0 heads_default wzcagccccccccccccuccccccccccrccqccccccccccccccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab 1885,1944 329,330 0,0" << std::endl;
          *fs << "633 div:int (none) 314 1 1 99 96 383 0.250653 0 -1 -1 7 0 heads_default wzcagcccccccccccccccccccccckrccccccccccyccccwcccccccccccccccccccccbcclcccccccccccccccccccczvfcaxgab 1827 33 0" << std::endl;
          *fs << "610 div:int (none) 390 1 1 98 95 379 0.250984 0 -1 -1 6 0 heads_default wzcagcccccccccccccccycccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfnaxgab 1779 188 0" << std::endl;
          *fs << "242 div:int (none) 46 3 3 98 94 376 0.249503 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccceccccrcccccccccccccccccccccccccccncccccccccccccccccccccacccccecccczvfcaxgab 1703,1762,1822 264,263,329 0,0,0" << std::endl;
          *fs << "587 div:int (none) 469 1 1 100 97 386 0.251295 0 -1 -1 5 0 heads_default wzcagccccjccccccwcccccccccccccecccccccccccccccccccccccccccccccxcccsccccccccccccccccccactccczvfcaxgab 1660 255 0" << std::endl;
          *fs << "403 div:int (none) 308 1 1 98 95 377 0.251324 0 -1 -1 7 0 heads_default wocagcccccccccccccccccccpcccrccccccccccccccccccccccccccccccccccccccccccccccdccacccccrcccczvfcaxgab 1580 773 0" << std::endl;
          *fs << "449 div:int (none) 352 1 1 100 98 384 0.252604 0 -1 -1 7 0 heads_default zcagccccccccqccvcccccchccccccccccgcccpcccccccccccccyccccccccsccccccccccuccaccccccccdcqccccczvfcaxgab 1407 672 0" << std::endl;
          *fs << "311 div:int (none) 238 1 1 98 95 376 0.251989 0 -1 -1 6 0 heads_default wzcatcccccccccccccccccccccclccwccccctcccicccccccccccccucccccccccccccccccccccccccccecccccczvfcaxgab 1405 1167 0" << std::endl;
          *fs << "371 div:int (none) 26 1 2 99 96 383 0.250218 0 -1 -1 3 0 heads_default wzcagcccccyccccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2198 279 0" << std::endl;
          *fs << "210 div:int (none) 26 2 5 100 97 387 0.250369 0 -1 -1 3 0 heads_default wzcagcccccccccccccccyccccccccccsccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2138,2199 215,215 0,0" << std::endl;
          *fs << "624 div:int (none) 283 1 1 98 85 369 0.230352 0 -1 -1 6 0 heads_default wzcagcfcccccccccccccccccccccccccccccccccrccccccccccccccccqccccvcccccccccciccccccccccccccczvfcaxgab 2126 132 0" << std::endl;
          *fs << "26 div:int (none) 16 1 6 100 97 388 0.25 0 -1 -1 2 0 heads_default wzcagcccccccccccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 2075 215 0" << std::endl;
          *fs << "578 div:int (none) 204 1 1 98 95 378 0.250991 0 -1 -1 6 0 heads_default wzcagcccccccccccmcccccccccccrccycccccccccccccccccccccccccccccccccccccccccjccccacccccccccczvfcaxgjb 2064 224 0" << std::endl;
          *fs << "486 div:int (none) 63 1 1 99 87 374 0.25 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccacccczccccccccccccccccccccccccccccccccucccccczvfcaxgab 2014 197 0" << std::endl;
          *fs << "509 div:int (none) 416 1 1 98 95 379 0.250653 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccwcnccccwcqcccccccccccccccccccccmccccccccccccccccccccccccccuccccccqvfcaxgab 2013 363 0" << std::endl;
          *fs << "95 div:int (none) 54 5 8 98 95 377 0.251856 0 -1 -1 5 0 heads_default wzcagcccccccccccccccjccccccccccccccccccccccccyccccccccccccccccccmccccccccccccccccdmcccccczvfcaxgab 2011,2012,2071,2072,2130 272,272,206,305,239 0,0,0,0,0" << std::endl;
          *fs << "463 div:int (none) 36 2 2 98 89 373 0.25 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccmccccccccczccccccczvfcaxgab 1946,2007 264,296 0,0" << std::endl;
          *fs << "601 div:int (none) 372 1 1 101 97 386 0.250646 0 -1 -1 4 0 heads_default wzcagcccccccccccccccctcccccccceccccccccccccsccccccccccccccccccccccccccccccccccccccccccccccczvfacaxgab 1781 231 0" << std::endl;
          *fs << "555 div:int (none) 361 1 1 100 97 384 0.251948 0 -1 -1 6 0 heads_default wzcagwccccnccqcccccccccccccccccccccccckcccccccccccccrcccccccsccccccccccuccccccwccccdcqccccczvfcaxgqb 1468 297 0" << std::endl;
          *fs << "348 div:int (none) 103 1 1 99 96 381 0.251744 0 -1 -1 5 0 heads_default gzcagcccdccccccccccccccccccccdccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccpczvfcaxgab 1414 1023 0" << std::endl;
          *fs << "376 div:int (none) 296 2 4 98 95 375 0.253333 0 -1 -1 8 0 heads_default wzcagcctccccccrccccccccccccccpccccgccccacccxaccccccccclcccccccccccccccccccccccccccucccccczvfcaxgab 2076,2137 267,267 0,0" << std::endl;
          *fs << "284 div:int (none) 204 4 5 98 86 368 0.244272 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccccrccycccccccccccccccccccccccccccccccccccccccccjccczacccccccccczvfcaxgab 2062,2063,2122,2124 159,192,192,159 0,0,0,0" << std::endl;
          *fs << "606 div:int (none) 377 1 1 98 82 364 0.243316 0 -1 -1 7 0 heads_default wzcagcccccccczccccccccccccccrccyccocccccccccfcccczccccccccccccccccccccccccccceacccccccccczvfcaxgab 2004 149 0" << std::endl;
          *fs << "629 div:int (none) 396 1 1 99 86 372 0.230563 0 -1 -1 5 0 heads_default wicagccccccfcccccccccccccccccyccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab 1888 82 0" << std::endl;
          *fs << "8 div:int (none) 6 1 14 98 95 380 0.25 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab 1824 329 0" << std::endl;
          *fs << "491 div:int (none) 165 1 1 98 95 379 0.250132 0 -1 -1 4 0 heads_default wzcmgcccccccccccccccccccccccccccccccccccicccccccccccccccccccccccccccccccccycccccccccccccczvfcaxgab 1649 637 0" << std::endl;
          *fs << "422 div:int (none) 116 1 1 98 93 570 0.25066 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccccccccccccccccccccccccacccchrcccczvfcaxgab 1642 164 0" << std::endl;
          *fs << "583 div:int (none) 489 1 1 101 48 336 0.250646 0 -1 -1 4 0 heads_default cccccccccmccccccccccccccccccccccccccccccczvfciaxgabwzcagcccccccccccccccccccccccccepcccccccccccccccccc 1596 119 0" << std::endl;
          *fs << "353 div:int (none) 102 1 1 100 97 386 0.250776 0 -1 -1 3 0 heads_default ezcagccccccccscccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccncdccccccccccczvfcaxgab 1590 1087 0" << std::endl;
          *fs << "31 div:int (none) 9 3 6 99 96 385 0.249352 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccqccccczvfcaxgab 1587,1588,1648 296,297,330 0,0,0" << std::endl;
          *fs << "514 div:int (none) 319 1 1 198 97 386 0.250646 0 -1 -1 3 0 heads_default aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaawzcagccccccccsiccccccccccccccccccccccccccccccccchcccccccccwccccccccccccccccccnccccccceccccczvfcaxgab 1528 363 0" << std::endl;
          *fs << "192 div:int (none) 75 1 4 98 94 377 0.248677 0 -1 -1 4 0 heads_default wzcagcccccccccccccccciccccccccccccccccccicccccccccccccccccccccccccdcccccccccccccccccccccczvfcaxgab 1526 330 0" << std::endl;
          *fs << "399 div:int (none) 233 2 2 98 95 377 0.251323 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccccdcnacccccrcccczvfcaxgab 1522,1581 66,65 0,0" << std::endl;
          *fs << "445 div:int (none) 60 2 2 100 97 384 0.251948 0 -1 -1 4 0 heads_default wzcagcccdccccccyccccccmcccccccccccccccccccpccccccccccccccccccccccccpccccccccccccccccccccccczvfcaxgab 1474,1475 308,308 0,0" << std::endl;
          *fs << "527 div:int (none) 337 1 1 98 96 381 0.251309 8 -1 -1 5 0 heads_default wzcagccdccccccccccccccccccccccccmcccccccccccccccccccccdcccccccccccccccccccccccccccucccccczvfcaxgab 2373 363 0" << std::endl;
          *fs << "435 div:int (none) 337 2 2 99 96 381 0.251969 0 -1 -1 5 0 heads_default wzcagccdccccccccccccccccccccccccsmcccccccccccccccccccccdcccccccccccccccccccccccccccucccccczvfcaxgab 2372,2433 296,363 0,0" << std::endl;
          *fs << "389 div:int (none) 210 1 1 100 97 386 0.250646 0 -1 -1 4 0 heads_default wzcagcccccccccccccccyccccccccccscccccccccccccccccccccccccccccccccckcccccccccccccccccccccccnzvfcaxgab 2016 248 0" << std::endl;
          *fs << "481 div:int (none) 298 1 2 99 96 381 0.251309 8 -1 -1 6 0 heads_default wzcagcccccccccccccccycccccccyccccccccccccccccccccccjccccccccccccccccccccccccccccxclccccccnzvfcaxgab 1958 239 0" << std::endl;
          *fs << "67 div:int (none) 26 1 4 99 96 384 0.25 0 -1 -1 3 0 heads_default wzcagcccccccccccccccyccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnzvfcaxgab 1955 182 0" << std::endl;
          *fs << "596 div:int (none) 486 1 1 100 87 374 0.25 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccacccczccccccccccccccccccccccceccccccccucccccczvfcaxgabc 1953 198 0" << std::endl;
          *fs << "366 div:int (none) 287 1 2 100 97 386 0.251309 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccpuccccccccccccccccccccccccccccccccccccccccccccccccczclyccccccccccccczvfcaxgab 1892 303 0" << std::endl;
          *fs << "504 div:int (none) 45 1 1 99 48 278 0.252632 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccpcrcccccccccrcccccwcccccccccccccccccccccccclccccccccccccccccgccczvfcxxgab 1705 65 0" << std::endl;
          *fs << "458 div:int (none) 18 1 1 100 97 388 0.249743 0 -1 -1 2 0 heads_default wzctgccccccccccccccccccccccccccccccccccccccccccccccccccccccccsccccccccccccccccccccccccccccczvfcaxgab 1653 627 0" << std::endl;
          *fs << "320 div:int (none) 173 1 1 99 95 380 0.249337 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccccrccccchccccccccccccccccccccccnccccccccccccccqccccccacccccecccczvfcaxgab 1640 362 0" << std::endl;
          *fs << "619 div:int (none) 493 1 1 100 88 376 0.234043 0 -1 -1 4 0 heads_default wzcagccccoccccccccccccccpccccccccccccccccccccccccccctpcccccsccccccccccccccycccccccccccccccczvfcaxgab 1594 166 0" << std::endl;
          *fs << "113 div:int (none) 71 1 1 100 48 278 0.252632 0 -1 -1 7 0 heads_default cccccccccccccccccccclccccccccccccccccgccczvfcwxgabwzcagcccccccccccccccccpccccpcrcccccccccrcccccwcccc 1585 1191 0" << std::endl;
          *fs << "329 div:int (none) 184 1 1 99 96 382 0.250653 0 -1 -1 6 0 heads_default wzcagcccccwccccccccccjcccccccccccccccjcccccccccccccccccccccccccccccxcccccccccccccccucccccczbfcaxgab 2193 1127 0" << std::endl;
          *fs << "375 div:int (none) 26 2 2 100 97 388 0.25 0 -1 -1 3 0 heads_default wzcagcccccccccccccccyccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccxccccnzvfcaxgab 2135,2195 279,279 0,0" << std::endl;
          *fs << "536 div:int (none) 415 1 1 111 105 416 0.251852 0 -1 -1 9 0 heads_default aaaaaaaaaawzcazccccccecccccccccccchcccccccccccuccccocccccccccccccccccclcccccccccccccccccccccccqccccccyzvfcaxgab 2129 352 0" << std::endl;
          *fs << "283 div:int (none) 128 1 3 98 85 369 0.230352 0 -1 -1 5 0 heads_default wzcagcfccccccccccccccccccccccccccccccccccccccccccccccccccqccccccccccccccciccccccccccccccczvfcaxgab 2127 131 0" << std::endl;
          *fs << "398 div:int (none) 231 1 2 99 92 380 0.240209 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccfccccncccccccccccccccccccccccccccczvfcaxgab 1771 98 0" << std::endl;
          *fs << "628 div:int (none) 106 1 1 99 87 377 0.230388 8 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccccjccccccccccccccccccfcccccccccccccckccccccccccccccccccczvfaxgab 1714 66 0" << std::endl;
          *fs << "490 div:int (none) 21 1 1 100 97 387 0.25 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccdccccccccccccccsccccccccccccccccccccccccqccccczvfcaxgab 1709 230 0" << std::endl;
          *fs << "582 div:int (none) 490 1 1 100 97 387 0.25 0 -1 -1 4 0 heads_default wzeagccccccccccccccccccccccccccccccccccccccccdccccccccccccccsccccccccccccccccccccccccqccccczvfcaxgab 1650 231 0" << std::endl;
          *fs << "421 div:int (none) 320 1 1 198 95 380 0.249337 0 -1 -1 7 0 heads_default aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaawzcaxcccccccccccccccccccccccrccccchccccccccccccccccccccccncuccccccccccccqccccccacccccecccczvfcaxgab 1639 728 0" << std::endl;
          *fs << "605 div:int (none) 222 1 1 100 97 388 0.249357 0 -1 -1 4 0 heads_default wzcagrccccbcccccccccwcccccccxcccccccccccccccactcccccccccccccccccbcccccccccccccccccccccccccczvfcaxgab 1592 198 0" << std::endl;
          *fs << "227 div:int (none) 109 0 2 98 95 378 0.250793 0 -1 -1 5 0 heads_default wzcagcccrccccccccccccccccccccwccccccccccicciccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "326 div:int (none) 242 0 1 98 94 376 0.25 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccceccccrcccccccccccccccccccccccccccncccccccccbcccccccccccacccccecccczvfcaxgab" << std::endl;
          *fs << "308 div:int (none) 233 0 1 98 95 377 0.251323 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccpcccrccccccccccccccccccccccccccccccccccccccccccccccdccacccccrcccczvfcaxgab" << std::endl;
          *fs << "495 div:int (none) 283 0 1 98 85 368 0.230352 0 -1 -1 6 0 heads_default wzcagcfccccccccccccccccccccccchccccccccccccccxcccccccccccqccccccccccccccciccccccccccccccczvfcaxgab" << std::endl;
          *fs << "36 div:int (none) 4 0 8 98 95 380 0.249836 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccmccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "297 div:int (none) 223 0 1 98 95 378 0.251309 0 -1 -1 6 0 heads_default wzcagcccccccccccccccyoccccccyccccccqccccccccccccccccccccccccccccccccccccccccccccclccccccnzvfcaxgab" << std::endl;
          *fs << "447 div:int (none) 94 0 1 100 97 384 0.251948 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccscccccccccccpcccccccccccccccccccccsccccckccccucccccccccccccqccccczvfcaxgab" << std::endl;
          *fs << "416 div:int (none) 328 0 1 98 95 379 0.250653 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccwccccccccqcccccccccccccccccccccmccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "272 div:int (none) 143 0 2 100 97 384 0.252276 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccciccccccccccccccdbccccccccccccccccccccccccccccccccgccccccpcicccccczvfcaxgab" << std::endl;
          *fs << "269 div:int (none) 191 0 5 100 97 386 0.251035 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccceccccccccccccccccccccccccccccuccccccccccccccccccccccctccccccczvfcaxgab" << std::endl;
          *fs << "23 div:int (none) 2 0 6 98 95 380 0.249815 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccicccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "223 div:int (none) 162 0 1 99 96 382 0.251309 0 -1 -1 5 0 heads_default wzcagcccccccccccccccyoccccccyccccccqcccccccccccccccccccccccccccccccccccccccccccccclccccccnzvfcaxgab" << std::endl;
          *fs << "204 div:int (none) 53 0 2 98 95 378 0.250991 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrccycccccccccccccccccccccccccccccccccccccccccjccccacccccccccczvfcaxgab" << std::endl;
          *fs << "304 div:int (none) 225 0 1 100 94 379 0.247368 0 -1 -1 5 0 heads_default wzcagcccccccccccccccyccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccicccczcccnzvfcaxgabq" << std::endl;
          *fs << "141 div:int (none) 94 0 1 100 49 284 0.251948 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccscccccccccccecccccccccccccccccccccsccccccccccucccccccccccccqccccczvfcyxgab" << std::endl;
          *fs << "262 div:int (none) 194 0 1 100 97 384 0.251948 0 -1 -1 5 0 heads_default wzcagccccccccscccccccccccccccccdccccccccccbccccccccccccccccccccccccccccccccccncdccccncccccczvfcaxgab" << std::endl;
          *fs << "150 div:int (none) 89 0 2 102 99 394 0.251295 0 -1 -1 4 0 heads_default aawzccgcccdcccccccccccccccdcccccccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "415 div:int (none) 324 0 1 108 105 416 0.251852 0 -1 -1 8 0 heads_default aaaaaaaawzcagccccccecccccccccccchcccccccccccuccccocccccccccccccccccclcccccccccccccccccccccccqcccccczvfcaxgab" << std::endl;
          *fs << "21 div:int (none) 9 0 6 100 97 388 0.249786 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccsccccccccccccccccccccccccqccccczvfcaxgab" << std::endl;
          *fs << "334 div:int (none) 14 0 1 97 94 376 0.25 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccycccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "114 div:int (none) 29 0 2 99 96 383 0.250653 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccwcccccccccccccccccccccbcclcccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "177 div:int (none) 82 0 1 98 95 377 0.251323 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccwccccctcccicccccccccccccucccccccccccccccccccccccccccecccccczvfcaxgab" << std::endl;
          *fs << "45 div:int (none) 29 0 4 99 96 380 0.251501 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccpcrcccccccccrcccccwcccccccccccccccccccccccclccccccccccccccccgccczvfcaxgab" << std::endl;
          *fs << "392 div:int (none) 226 0 1 100 96 383 0.250653 0 -1 -1 5 0 heads_default wzcagcccccqcccccccccycccccccccccccccccccacccccoccccccccccecacccclccccccccdccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "231 div:int (none) 40 0 1 100 92 383 0.230159 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccfcccccncccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "18 div:int (none) 1 0 4 100 97 388 0.249766 0 -1 -1 1 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccsccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "163 div:int (none) 107 0 2 98 95 379 0.250655 0 -1 -1 5 0 heads_default wzcagccccccccccccccccccccccccpccccccccccccccacccccccccccccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "185 div:int (none) 42 0 3 101 98 391 0.250646 0 -1 -1 2 0 heads_default wzcagccccccccscccccccccccccccccccccccccccccccccccccccccccccccccccccqccccccccccnccccccccccccczvfcaxgab" << std::endl;
          *fs << "382 div:int (none) 120 0 1 98 48 269 0.233062 0 -1 -1 6 0 heads_default wzcagcbccccccccccccccccccccxcccccccccccgcccccccccccocccccccccccccccccccmczccccccccccccccczvfcmxgab" << std::endl;
          *fs << "225 div:int (none) 67 0 2 99 94 380 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccyccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccccccczcccnzvfcaxgab" << std::endl;
          *fs << "379 div:int (none) 203 0 1 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzcagcccccccccccwcccccccccccccecccccccccccccccccccccccccccccccccccscccccccccccccccccccctccczvfcaxgab" << std::endl;
          *fs << "328 div:int (none) 193 0 1 99 96 383 0.250653 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccwccccccccqcccccccccccccccccccccmcccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "324 div:int (none) 241 0 1 105 102 405 0.251256 0 -1 -1 7 0 heads_default aaaaaawzcagccccccecccccccccccchcccccccccccccccocccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "102 div:int (none) 42 0 4 100 97 386 0.250924 0 -1 -1 2 0 heads_default wzcagccccccccscccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccncdccccccccccczvfcaxgab" << std::endl;
          *fs << "14 div:int (none) 4 0 5 98 95 380 0.249821 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccyccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "253 div:int (none) 184 0 1 99 96 381 0.251309 0 -1 -1 6 0 heads_default wzcagcccccwccccccccccjcccccccccccccccjcccccccccccccccccccccccccccccxcccccccccccccccuccccuczvfcaxgab" << std::endl;
          *fs << "340 div:int (none) 262 0 1 100 49 332 0.252604 0 -1 -1 6 0 heads_default wzcagccccccccscccccccccccccccccdccccccccccbccccccccccccccccccccccccccccccpcccncdccccncccccczvlcaxgab" << std::endl;
          *fs << "352 div:int (none) 148 0 1 101 98 384 0.252604 0 -1 -1 6 0 heads_default wzcagccccccccqccvcccccccccccccccccgcccpcccccccccccccyccccccccsccccccccccuccaccccccccdcqccccczvfcaxgab" << std::endl;
          *fs << "238 div:int (none) 177 0 1 98 95 376 0.251989 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccclccwccccctcccicccccccccccccucccccccccccccccccccccccccccecccccczvfcaxgab" << std::endl;
          *fs << "123 div:int (none) 74 0 2 99 96 383 0.25 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccctccccccccccccccccccxcccccccccccccsccccccccccccaccccccccccccccczvfcaxgab" << std::endl;
          *fs << "233 div:int (none) 116 0 1 98 95 378 0.25066 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccccdccacccccrcccczvfcaxgab" << std::endl;
          *fs << "88 div:int (none) 41 0 2 100 97 385 0.251295 0 -1 -1 4 0 heads_default wzcagccccccccqccccccccccccccccccccccccpcccccccccccccccccccccsccccccccccucccccccccccdcqccccczvfcaxgab" << std::endl;
          *fs << "53 div:int (none) 8 0 2 98 95 379 0.250528 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrccyccccccccccccccccccccccccccccccccccccccccccccccacccccccccczvfcaxgab" << std::endl;
          *fs << "168 div:int (none) 46 0 1 98 94 377 0.249337 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccacrcccccccccccccccccccccccccccncccccccccccccccccccccacccccecccczvfcaxgab" << std::endl;
          *fs << "236 div:int (none) 175 0 1 99 96 381 0.250653 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccchcccctcccccccccccccccckcxcccccccccccccscccbccccccccaccccccccccccccczvfcaxgab" << std::endl;
          *fs << "173 div:int (none) 46 0 2 98 94 377 0.249337 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccnccccccccccccccqccccccacccccecccczvfcaxgab" << std::endl;
          *fs << "226 div:int (none) 104 0 2 100 96 383 0.251295 0 -1 -1 4 0 heads_default wzcagcccccccccccccccycccccccccccccccccccacccccoccccccccccecacccclccccccccdccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "213 div:int (none) 15 0 2 99 86 373 0.240281 0 -1 -1 3 0 heads_default wzcagccccccfccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "80 div:int (none) 1 0 2 100 88 378 0.249357 0 -1 -1 1 0 heads_default wzcagcccccccccccccccccccpccccccccccccccccccccccccccccccccccccccccccccccccczcccccccccccccccczvfcaxgab" << std::endl;
          *fs << "128 div:int (none) 56 0 2 98 85 369 0.236901 0 -1 -1 4 0 heads_default wzcagcfcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccciccccccccccccccczvfcaxgab" << std::endl;
          *fs << "121 div:int (none) 52 0 3 100 97 387 0.250431 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccceccccccccccgcczvfcaxgab" << std::endl;
          *fs << "136 div:int (none) 53 0 2 98 91 374 0.25066 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccccccrccyccccccccccccfcccczccccccccccccccccccccccccccccacccccccccczvfcaxgab" << std::endl;
          *fs << "245 div:int (none) 84 0 1 98 86 369 0.25066 0 -1 -1 5 0 heads_default wzcagcccccccccccccccccccczcccccccccccccccccccyccccccccccccccccccccccccccccccccccccpcccccczvfcaxgab" << std::endl;
          *fs << "217 div:int (none) 163 0 1 98 95 379 0.25066 0 -1 -1 6 0 heads_default wzcagccccccccccccccccccccccccpcccccccccaccccacccccccccccccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "84 div:int (none) 14 0 3 98 95 379 0.25055 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccyccccccccccccccccccccccccccccccccccccpcccccczvfcaxgab" << std::endl;
          *fs << "193 div:int (none) 10 0 2 99 96 383 0.250327 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccccccctcccccccccccccccccccccccccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "228 div:int (none) 108 0 1 100 97 383 0.251295 8 -1 -1 4 0 heads_default wzcagcccdccccccccccccccccccccccccccccccccccccccbcgccccgccccccccccccccccccccccccccccccccccvczvfcaxgab" << std::endl;
          *fs << "15 div:int (none) 2 0 10 99 96 384 0.249913 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "42 div:int (none) 1 0 4 100 97 387 0.250163 0 -1 -1 1 0 heads_default wzcagccccccccscccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccnccccccccccccczvfcaxgab" << std::endl;
          *fs << "12 div:int (none) 1 0 7 100 97 388 0.249852 0 -1 -1 1 0 heads_default wzcagcccccccccccccccccccccccccecccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "145 div:int (none) 36 0 1 98 95 379 0.25 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccclccccmccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "241 div:int (none) 178 0 1 103 100 398 0.251282 0 -1 -1 6 0 heads_default aaaawzcagccccccecccccccccccchcccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "109 div:int (none) 64 0 2 98 95 379 0.25066 0 -1 -1 4 0 heads_default wzcagcccrccccccccccccccccccccwccccccccccicccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "101 div:int (none) 1 0 1 100 97 387 0.249357 0 -1 -1 1 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccgccccccccicccccczvfcaxgab" << std::endl;
          *fs << "107 div:int (none) 63 0 1 99 96 383 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccccpccccccccccccccacccccccccccccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "77 div:int (none) 36 0 2 98 86 370 0.25 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccmczccccccccccccccczvfcaxgab" << std::endl;
          *fs << "40 div:int (none) 1 0 2 100 87 378 0.245517 0 -1 -1 1 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccfcccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "133 div:int (none) 102 0 1 100 97 386 0.251295 0 -1 -1 3 0 heads_default wzcagccccccccsccccccccccccccccccccccccccccbccccccccccccccccccccccccccccccccccncdccccccccccczvfcaxgab" << std::endl;
          *fs << "191 div:int (none) 12 0 1 100 97 387 0.25 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccccccceccccccccccccccccccccccccccccuccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "111 div:int (none) 24 0 2 100 97 389 0.249357 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccxcccccccccccccccacccccccccccccccccccbcccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "178 div:int (none) 119 0 1 101 98 390 0.251309 0 -1 -1 5 0 heads_default aawzcagccccccecccccccccccchcccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "174 div:int (none) 8 0 1 98 95 377 0.25 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccccccccccccccccccccccvcccccccccccccacccccccccczvfcaxgab" << std::endl;
          *fs << "108 div:int (none) 25 0 1 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzcagcccdccccccccccccccccccccccccccccccccccccccbcgccccgcccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "104 div:int (none) 43 0 1 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzcagcccccccccccccccycccccccccccccccccccacccccoccccccccccccccccclcccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "175 div:int (none) 123 0 1 99 96 383 0.250653 0 -1 -1 4 0 heads_default wzcagccccccccccccccccccccccctccccccccccccccccccxcccccccccccccscccbccccccccaccccccccccccccczvfcaxgab" << std::endl;
          *fs << "119 div:int (none) 81 0 1 99 96 382 0.250653 0 -1 -1 4 0 heads_default wzcagccccccecccccccccccchcccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "10 div:int (none) 2 0 4 99 96 384 0.249784 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "89 div:int (none) 25 0 1 100 97 386 0.250646 0 -1 -1 3 0 heads_default wzccgcccdcccccccccccccccdcccccccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "52 div:int (none) 1 0 2 100 97 388 0.249678 0 -1 -1 1 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccceccccccccccccczvfcaxgab" << std::endl;
          *fs << "74 div:int (none) 18 0 2 99 96 384 0.25 0 -1 -1 2 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccxcccccccccccccsccccccccccccaccccccccccccccczvfcaxgab" << std::endl;
          *fs << "25 div:int (none) 5 0 3 100 97 387 0.250277 0 -1 -1 2 0 heads_default wzcagcccdcccccccccccccccccccccccccccccccccccccccccccccgcccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "54 div:int (none) 14 0 2 98 95 378 0.250331 0 -1 -1 4 0 heads_default wzcagcccccccccccccccjccccccccccccccccccccccccycccccccccccccccccccccccccccccccccccdmcccccczvfcaxgab" << std::endl;
          *fs << "82 div:int (none) 23 0 1 98 95 378 0.25 0 -1 -1 3 0 heads_default wzcagccccccccccccccccccccccccccccccctcccicccccccccccccucccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "81 div:int (none) 15 0 1 99 96 383 0.25 0 -1 -1 3 0 heads_default wzcagcccccceccccccccccccccccccccccccccccccccccccccccccccccclcccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "132 div:int (none) 61 0 1 99 96 383 0.250653 0 -1 -1 4 0 heads_default wzcagcccccwccccccccccjcccccccccccccccccccccccccccccccccccccccccccccxcccccccccccccccucccccczvfcaxgab" << std::endl;
          *fs << "1 div:ext (none) (none) 0 12 100 97 389 0.202603 0 -1 -1 0 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "64 div:int (none) 23 0 1 98 95 379 0.25 0 -1 -1 3 0 heads_default wzcagcccrcccccccccccccccccccccccccccccccicccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "71 div:int (none) 45 0 1 99 48 278 0.252632 0 -1 -1 6 0 heads_default wzcagcccccccccccccccccccccpcrcccccccccrcccccwcccccccccccccccccccccccclccccccccccccccccgccczvfcwxgab" << std::endl;
          *fs << "27 div:int (none) 5 0 2 100 97 387 0.250161 0 -1 -1 2 0 heads_default wzcagcccdcccccccccccccccccccccccccccccccccpcccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "29 div:int (none) 17 0 2 99 96 383 0.250218 0 -1 -1 4 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccwcccccccccccccccccccccccclcccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "43 div:int (none) 16 0 1 100 97 387 0.25 0 -1 -1 2 0 heads_default wzcagcccccccccccccccycccccccccccccccccccccccccocccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "9 div:int (none) 1 0 2 100 97 389 0.249357 0 -1 -1 1 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccqccccczvfcaxgab" << std::endl;
          *fs << "16 div:int (none) 1 0 1 100 97 388 0.249357 0 -1 -1 1 0 heads_default wzcagcccccccccccccccycccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "5 div:int (none) 1 0 2 100 97 388 0.249571 0 -1 -1 1 0 heads_default wzcagcccdcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "2 div:int (none) 1 0 2 99 96 385 0.249357 0 -1 -1 1 0 heads_default wzcagccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "6 div:int (none) 2 0 1 99 96 384 0.249351 0 -1 -1 2 0 heads_default wzcagcccccccccccccccccccccccrccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
          *fs << "17 div:int (none) 6 0 1 99 96 384 0.25 0 -1 -1 3 0 heads_default wzcagcccccccccccccccccccccccrcccccccccccccccwccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
        }
        
        file.Close();

        // Update events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << "u begin LoadPopulation detail.spop" << std::endl;
        file.Close();
      }
      
      
      void CreateDefaultFreezer(const Apto::String& path)
      {
        Apto::String entry_path;
        
        // Default Config
        entry_path = Apto::FileSystem::PathAppend(path, "c0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultConfig(entry_path, "@default");
        
        // Default Genome
        entry_path = Apto::FileSystem::PathAppend(path, "g0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultGenome(entry_path);
                
        // Sample Worlds
        entry_path = Apto::FileSystem::PathAppend(path, "w0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultConfig(entry_path, "@example");
        WriteExamplePopulation(entry_path, "@example");
        
        entry_path = Apto::FileSystem::PathAppend(path, "w1");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultConfig(entry_path, "Update200");
        WriteExamplePopulation(entry_path, "Update200");
      }
      
      
      bool SaveConfig(cWorld* world, const Apto::String& path)
      {
        Apto::String file_path;
        cFile file;
        std::fstream* fs;
        
        
        // avida.cfg
        file_path = Apto::FileSystem::PathAppend(path, "avida.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "WORLD_X " << world->GetConfig().WORLD_X.Get() << std::endl;
        *fs << "WORLD_Y " << world->GetConfig().WORLD_Y.Get() << std::endl;
        *fs << "COPY_MUT_PROB " << world->GetConfig().COPY_MUT_PROB.Get() << std::endl;
        *fs << "BIRTH_METHOD " << world->GetConfig().BIRTH_METHOD.Get() << std::endl;
        *fs << "RANDOM_SEED " << world->GetConfig().RANDOM_SEED.Get() << std::endl;
        *fs << "#include instset.cfg" << std::endl;
        
        file.Close();
        
        
        // write instset.cfg
        WriteInstSet(path);
        
        
        // events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();        
        file.Close();
        
        
        // environment.cfg
        file_path = Apto::FileSystem::PathAppend(path, "environment.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        // @TODO 
        cEnvironment& env = world->GetEnvironment();
        *fs << "REACTION  NOT  not   process:value=" << env.GetReactionValue("NOT")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NAND nand  process:value=" << env.GetReactionValue("NAND") << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  AND  and   process:value=" << env.GetReactionValue("AND")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ORN  orn   process:value=" << env.GetReactionValue("ORN")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  OR   or    process:value=" << env.GetReactionValue("OR")   << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ANDN andn  process:value=" << env.GetReactionValue("ANDN") << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NOR  nor   process:value=" << env.GetReactionValue("NOR")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  XOR  xor   process:value=" << env.GetReactionValue("XOR")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  EQU  equ   process:value=" << env.GetReactionValue("EQU")  << ":type=pow  requisite:max_count=1" << std::endl;
        
        file.Close();
        
        return true;
      }
      
      
      bool SavePopulation(cWorld* world, const Apto::String& path)
      {
        Apto::String file_path;
        cFile file;
        std::fstream* fs;
        
        // Save Population
        file_path = Apto::FileSystem::PathAppend(path, "detail.spop");
        if (!world->GetPopulation().SavePopulation((const char*)file_path, true)) return false;
        
        // Update events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << "u begin LoadPopulation detail.spop" << std::endl;
        file.Close();
        
        return true;
      }
      
    }
  }
}

Avida::Viewer::Freezer::Freezer(const Apto::String& dir) : m_dir(dir), m_opened(false)
{
  // Initialize next identifiers
  m_next_id[0] = m_next_id[1] = m_next_id[2] = -1;
  
  
  // Check for existing freezer dir
  if (!Apto::FileSystem::IsDir(m_dir)) {
    if (!Apto::FileSystem::MkDir(m_dir)) return;
    
    // Set up defaults
    Private::CreateDefaultFreezer(m_dir);
  }
    
  // Open entries
  Apto::Array<Apto::String, Apto::Smart> direntries;
  if (!Apto::FileSystem::ReadDir(m_dir, direntries)) return;
  
  for (int i = 0; i < direntries.GetSize(); i++) {
    if (direntries[i].GetSize() < 2) continue;
    
    Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, direntries[i]);
    if (!Apto::FileSystem::IsDir(full_path)) continue;
    
    int identifier = Apto::StrAs(direntries[i].Substring(1));
    
    // Attempt to read name file
    Apto::String name = direntries[i];
    Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
    if (Apto::FileSystem::IsFile(name_path)) {
      // Open name file and read contents
      cFile file;
      file.Open((const char*)name_path, std::ios::in);
      cString line;
      file.ReadLine(line);
      file.Close();
      
      line.Trim();
      name = line;
    }
    
    FreezerObjectType t;
    switch (direntries[i][0]) {
      case 'c': t = CONFIG; break;
      case 'g': t = GENOME; break;
      case 'w': t = WORLD;  break;
      default: continue;
    }
    m_next_id[t] = (m_next_id[t] <= identifier) ? (identifier + 1) : m_next_id[t];
    m_entries[t].Push(Entry(name, direntries[i]));
  }
  
  m_opened = true;
}

Avida::Viewer::Freezer::~Freezer()
{
  if (!m_opened) return;
  
  // Search for inactive entries and remove them from the freezer
  for (int t = CONFIG; t <= WORLD; t++) {
    for (int i = 0; i < m_entries[t].GetSize(); i++) {
      if (!m_entries[t][i].active) {
        Apto::FileSystem::RmDir(Apto::FileSystem::PathAppend(m_dir, m_entries[t][i].path), true);
      }
    }
  }
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveConfig(cWorld* world, const Apto::String& name)
{
  if (!m_opened) return FreezerID(CONFIG, -1);

  Apto::String entry_path = Apto::FormatStr("c%d", m_next_id[CONFIG]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(CONFIG, -1);
  
  // Attempt to Save Config
  if (!Private::SaveConfig(world, full_path)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);    
  }
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  cFile file;
  if (!file.Open((const char*)name_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[CONFIG].Push(Entry(name, entry_path));
  return FreezerID(CONFIG, m_entries[CONFIG].GetSize() - 1);
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveGenome(GenomePtr genome, const Apto::String& name)
{
  if (!m_opened) return FreezerID(GENOME, -1);
  
  Apto::String entry_path = Apto::FormatStr("g%d", m_next_id[GENOME]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(GENOME, -1);
  
  // Attempt to Save Genome
  Apto::String genome_path = Apto::FileSystem::PathAppend(full_path, "genome.seq");
  cFile file;
  if (!file.Open((const char*)genome_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(GENOME, -1);
  }
  
  std::fstream& gfs = *file.GetFileStream();
  gfs << genome->AsString() << std::endl;
  
  file.Close();
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  if (!file.Open((const char*)name_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(GENOME, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[GENOME].Push(Entry(name, entry_path));
  return FreezerID(GENOME, m_entries[GENOME].GetSize() - 1);  
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveWorld(cWorld* world, const Apto::String& name)
{
  if (!m_opened) return FreezerID(WORLD, -1);

  Apto::String entry_path = Apto::FormatStr("w%d", m_next_id[WORLD]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(WORLD, -1);
  
  // Attempt to Save Config
  if (!Private::SaveConfig(world, full_path)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);    
  }
  
  // Attempt to Save Population
  if (!Private::SavePopulation(world, full_path)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);    
  }
    
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  cFile file;
  if (!file.Open((const char*)name_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(WORLD, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[WORLD].Push(Entry(name, entry_path));
  
  FreezerID new_id(WORLD, m_entries[WORLD].GetSize() - 1);
  SaveAttachment(new_id, "update", Apto::AsStr(world->GetStats().GetUpdate()));
  return new_id;
}


bool Avida::Viewer::Freezer::InstantiateWorkingDir(FreezerID entry_id, const Apto::String& working_directory) const
{
  if (!m_opened) return false;

  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;

  // Copy contained files to the destination directory
  Apto::String src_path = Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
  Apto::FileSystem::CpDir(src_path, working_directory);
  
  return true;
}

Avida::GenomePtr Avida::Viewer::Freezer::InstantiateGenome(FreezerID entry_id) const
{
  if (!m_opened) return GenomePtr();
  if (entry_id.type != GENOME || entry_id.identifier >= m_entries[GENOME].GetSize()) return GenomePtr();
  
  Apto::String seq_str;
  if (Apto::FileSystem::IsFile(PathOf(entry_id))) {
    // Open name file and read contents
    cFile file;
    file.Open((const char*)PathOf(entry_id), std::ios::in);
    cString line;
    if (!file.Eof() && file.ReadLine(line)) seq_str = line;
    file.Close();
  }

  return GenomePtr(new Genome(seq_str));
}


Apto::String Avida::Viewer::Freezer::PathOf(FreezerID entry_id) const
{
  // Check if it is a genome, and if so return the path to the file itself
  if (entry_id.type == GENOME)
  {
    Apto::String filepath = Apto::FileSystem::PathAppend(m_entries[entry_id.type][entry_id.identifier].path, "genome.seq");
    return Apto::FileSystem::PathAppend(m_dir, filepath);
  }
  
  // For all others, return the path of the containing directory
  return Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
}



bool Avida::Viewer::Freezer::SaveAttachment(FreezerID entry_id, const Apto::String& name, const Apto::String& value)
{
  if (!IsValid(entry_id) || entry_id.type == GENOME) return false;
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(PathOf(entry_id), name);
  cFile file;
  if (!file.Open((const char*)name_path, std::ios::out)) {
    return false;
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << value;
  
  file.Close();
  
  return true;
}

Apto::String Avida::Viewer::Freezer::LoadAttachment(FreezerID entry_id, const Apto::String& name)
{
  if (!IsValid(entry_id) || entry_id.type == GENOME) return "";
  
  Apto::String rtn;
  Apto::String name_path = Apto::FileSystem::PathAppend(PathOf(entry_id), name);
  if (Apto::FileSystem::IsFile(name_path)) {
    // Open name file and read contents
    cFile file;
    file.Open((const char*)name_path, std::ios::in);
    cString line;
    if (!file.Eof() && file.ReadLine(line)) rtn = line;
    while (!file.Eof() && file.ReadLine(line)) {
      rtn += "\n";
      rtn += line;
    }
    file.Close();
  }
  return rtn;
}



bool Avida::Viewer::Freezer::Rename(FreezerID entry_id, const Apto::String& name)
{
  if (!m_opened) return false;
  
  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;
  
  // Save new entry name to entry path
  cFile file;
  Apto::String name_path = Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
  name_path = Apto::FileSystem::PathAppend(name_path, "entryname.txt");
  if (!file.Open((const char*)name_path, std::ios::out)) {
    return false;
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Rename in memory entry
  m_entries[entry_id.type][entry_id.identifier].name = name;
  
  return true;
}

Apto::String Avida::Viewer::Freezer::NewUniqueNameForType(FreezerObjectType type, const Apto::String& name)
{
  int suffix = 1;
  Apto::String suggest = name;
  for (int i = 0; i < m_entries[type].GetSize(); i++) {
    if (m_entries[type][i].name == suggest) {
      suggest = name;
      suggest += " ";
      suggest += Apto::AsStr(suffix++);
      
      // reset index to re-check with new suggestion
      i = 0;
    }
  }
  
  return suggest;
}


void Avida::Viewer::Freezer::DuplicateFreezerAt(Apto::String destination)
{
  // Copy the workspace
  Apto::FileSystem::CpDir(m_dir, destination);
  
  // Delete any inactive items from new duplicate
  // Search for inactive entries and remove them from the freezer
  for (int t = CONFIG; t <= WORLD; t++) {
    for (int i = 0; i < m_entries[t].GetSize(); i++) {
      if (!m_entries[t][i].active) {
        Apto::FileSystem::RmDir(Apto::FileSystem::PathAppend(destination, m_entries[t][i].path), true);
      }
    }
  }
}
