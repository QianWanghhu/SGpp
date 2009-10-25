#############################################################################
# This file is part of pysgpp, a program package making use of spatially    #
# adaptive sparse grids to solve numerical problems                         #
#                                                                           #
# Copyright (C) 2007 Joerg Blank (blankj@in.tum.de)                         #
# Copyright (C) 2007 Richard Roettger (roettger@in.tum.de)                  #
# Copyright (C) 2008 Dirk Plueger (pflueged@in.tum.de)                      #
# Copyright (C) 2009 Alexander Heinecke (Alexander.Heinecke@mytum.de)       #
# Copyright (C) 2009 Valeriy Khakhutskyy (khakhutv@in.tum.de)               #
#                                                                           #
# pysgpp is free software; you can redistribute it and/or modify            #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 3 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# pysgpp is distributed in the hope that it will be useful,                 #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU Lesser General Public License for more details.                       #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with pysgpp; if not, write to the Free Software                     #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
# or see <http://www.gnu.org/licenses/>.                                    #
#############################################################################
from bin.data.DataSpecification import DataSpecification


import re
import gzip
from DataAdapter import DataAdapter
from bin.pysgpp import DataVector
from DataContainer import DataContainer


## Class implements the interface of DataAdapter for storing and restoring of input
# data into / from files in <a href="http://www.cs.waikato.ac.nz/~ml/weka/arff.html" 
# target="new">ARFF-format</a>.
class ARFFAdapter(DataAdapter):

    ## Filename associated with data
    filename = None


    ## Constructor
    #
    # @param filename: Filename as String
    def __init__(self, filename=""):
        self.filename = filename
    
    
    ## Store data into file
    # 
    # @param points: DataVector with points
    # @param values: DataVector with values, default None
    # @param attributes: dictionary with attributes of dataset, default None
    def save(self, points, values = None, attributes = None):

        fout = self.__gzOpen(self.filename, "w")
        dim = points.getDim()
        size = points.getSize()
        point = DataVector(dim)
        
        fout.write("@RELATION \"%s\"\n\n" % self.filename)
        
        hasclass = False
        if values != None:
            hasclass = True
        
        #@todo (khakhutv) get attributes from specification
        if attributes == None:
            for i in xrange(dim):
                fout.write("@ATTRIBUTE x%d NUMERIC\n" % i)
               
            if hasclass:
                fout.write("@ATTRIBUTE class NUMERIC\n")
        else:
            for key in attributes.keys():
                fout.write("@ATTRIBUTE %s %s\n" % (key, attributes[key]))
            
        fout.write("\n@DATA\n")
            

        for row in xrange(size):
            points.getRow(row, point)
            lout = []
            for col in xrange(dim):
                lout.append(point[col])
            if hasclass:
                lout.append(values[row])
            string = ",".join(str(i) for i in lout)
            fout.write(string+"\n")

        fout.close()


    ## Reads dataset from file
    #
    # @param name: String for category of data set (train or test), default "train"
    # @return DataContainer with data set
    def loadData(self, name = "train"):
        fin = self.__gzOpen(self.filename, "r")
        data = []
        classes = []
        hasclass = False
    
        # get the different section of ARFF-File
        for line in fin:
            sline = line.strip().lower()
            if sline.startswith("%") or len(sline) == 0:
                continue
    
            if sline.startswith("@data"):
                break
            
            if sline.startswith("@attribute"):
                value = sline.split()
                if value[1].startswith("class"):
                    hasclass = True
                else:
                    data.append([])
        
        #read in the data stored in the ARFF file
        for line in fin:
            sline = line.strip()
            if sline.startswith("%") or len(sline) == 0:
                continue
    
            values = sline.split(",")
            if hasclass:
                classes.append(float(values[-1]))
                values = values[:-1]
            for i in xrange(len(values)):
                data[i].append(float(values[i]))
                
        # cleaning up and return
        fin.close()
        
        dim = len(data)
        size = len(data[0])
        dataVector = DataVector(size, dim)
        tempVector = DataVector(dim)
        valuesVector = DataVector(size)
        for rowIndex in xrange(size):
            for colIndex in xrange(dim):
                tempVector[colIndex] = data[colIndex][rowIndex]
            dataVector.setRow(rowIndex, tempVector)
            valuesVector[rowIndex] = classes[rowIndex]
            
        return DataContainer(dataVector, valuesVector, name, self.filename)


    ## Loads attribute specification from file
    #
    # @return dictionary with attribute specification
    def loadSpecification(self):
        spec = DataSpecification()
        spec.setFilename(self.filename)
        fin = self.__gzOpen(self.filename, "r")
        for line in fin:
            sline = line.strip().lower()
            # comments:
            if sline.startswith("%") or len(sline) == 0:
                continue
            
            # attributes:
            elif sline.startswith("@attribute"):
                attrLine = line.strip().split()
                spec.addAttribute(attrLine[1], attrLine[2])
            
            # data block therefore no attributes will come
            elif sline.startswith("@data"):
                break
            
        return spec
    

    ## Opens a file. If the file ends with ".gz", automatically gzip compression
    # is used for the file. Returns the filedescriptor
    # @param filename
    # @param mode, default: "r" for read only
    # @return file descriptor
    def __gzOpen(self, filename, mode="r"):
        # gzip-file
        if re.match(".*\.gz$", filename):
            # mode set for binary data?
            if not mode[-1] == "b":
                mode += "b"
            fd = gzip.open(filename, mode)
        # non gzip-file
        else:
            fd = open(filename, mode)
        return fd


