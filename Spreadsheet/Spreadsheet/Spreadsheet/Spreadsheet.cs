using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SpreadsheetUtilities;
using System.ComponentModel;
using System.Xml;

namespace SpreadsheetTemplate
{

    /// <summary>
    /// A string is a valid cell name if and only if:
    ///   (1) its first character is an underscore or a letter
    ///   (2) its remaining characters (if any) are underscores and/or letters and/or digits
    /// </summary>
    public class Cell
    {
        public string Name { get; private set; }

        public object Contents { get; set; }

        public object CellValue { get; set; }

        public Cell(string name, object contents) : this(name, contents, s => true, s => s)
        { }

        public Cell(string name, object contents, Func<string, bool> Validator, Func<string, string> Normalizer)
        {
            if (IsValidName(name) && Validator(name))
            {
                Name = Normalizer(name);
                Contents = contents;
                //if (contents.GetType() == typeof(string))
                //    CellValue = (string)Contents;
                //if (contents.GetType() == typeof(double))
                //    CellValue = Contents;
                //if (contents.GetType() == typeof(Formula))
                //    CellValue = ((Formula)contents).Evaluate(s=>0/*MyDelegate*/);
            }
            else throw new InvalidCellNameException();
        }

        private bool IsValidName(string name)
        {
            bool result = true;
            bool startNums = false;
            if (name.Length > 0)
            {
                foreach (char character in name.Substring(0))
                {
                    if (char.IsDigit(character))
                        startNums = true;
                    if (startNums)
                    {
                        if (!char.IsDigit(character))
                            return false;
                    }
                    else if (!char.IsLetter(character)) return false;
                }
            }
            else result = false;
            return result;
        }

        public void WriteXml(XmlWriter writer)
        {
            string contents = "";
            if (Contents.GetType() == typeof(string))
                contents = (string)Contents;
            if (Contents.GetType() == typeof(double))
                contents = Contents.ToString();
            if (Contents.GetType() == typeof(Formula))
                contents = "=" + Contents.ToString();
            if (contents == "")
                throw new Exception("Now how the f*** did you do that? This should be impossible...");
            writer.WriteStartElement("cell");
            writer.WriteElementString("name", Name);
            writer.WriteElementString("contents", contents);
            writer.WriteEndElement();
        }

    }



    /// <summary>
    ///  An AbstractSpreadsheet object represents the state of a simple spreadsheet.  A 
    /// spreadsheet consists of an infinite number of named cells.
    /// 
    /// A string is a valid cell name if and only if:
    ///   (1) its first character is an underscore or a letter
    ///   (2) its remaining characters (if any) are underscores and/or letters and/or digits
    /// Note that this is the same as the definition of valid variable from the PS3 Formula class.
    /// 
    /// For example, "x", "_", "x2", "y_15", and "___" are all valid cell  names, but
    /// "25", "2x", and "&" are not.  Cell names are case sensitive, so "x" and "X" are
    /// different cell names.
    /// 
    /// A spreadsheet contains a cell corresponding to every possible cell name.  (This
    /// means that a spreadsheet contains an infinite number of cells.)  In addition to 
    /// a name, each cell has a contents and a value.  The distinction is important.
    /// 
    /// The contents of a cell can be (1) a string, (2) a double, or (3) a Formula.  If the
    /// contents is an empty string, we say that the cell is empty.  (By analogy, the contents
    /// of a cell in Excel is what is displayed on the editing line when the cell is selected.)
    /// 
    /// In a new spreadsheet, the contents of every cell is the empty string.
    ///  
    /// The value of a cell can be (1) a string, (2) a double, or (3) a FormulaError.  
    /// (By analogy, the value of an Excel cell is what is displayed in that cell's position
    /// in the grid.)
    /// 
    /// If a cell's contents is a string, its value is that string.
    /// 
    /// If a cell's contents is a double, its value is that double.
    /// 
    /// If a cell's contents is a Formula, its value is either a double or a FormulaError,
    /// as reported by the Evaluate method of the Formula class.  The value of a Formula,
    /// of course, can depend on the values of variables.  The value of a variable is the 
    /// value of the spreadsheet cell it names (if that cell's value is a double) or 
    /// is undefined (otherwise).
    /// 
    /// Spreadsheets are never allowed to contain a combination of Formulas that establish
    /// a circular dependency.  A circular dependency exists when a cell depends on itself.
    /// For example, suppose that A1 contains B1*2, B1 contains C1*2, and C1 contains A1*2.
    /// A1 depends on B1, which depends on C1, which depends on A1.  That's a circular
    /// dependency.
    /// </summary>
    public class Spreadsheet : AbstractSpreadsheet
    {
        public Spreadsheet() : base(s => true, s => s, "default")
        {
            this.SSDelegate = SSDel;
        }
        public Spreadsheet(Func<string, bool> validator, Func<string, string> normalizer, string version)
                           : base(validator, normalizer, version)
        {
            this.SSDelegate = SSDel;
        }
        public Spreadsheet(string filePath, Func<string, bool> validator,
                           Func<string, string> normalizer, string version)
                           : base(validator, normalizer, version)
        {
            this.SSDelegate = SSDel;
            ConstructFromSaved(filePath);
        }

        private Dictionary<string, Cell> nonEmptyCells = new Dictionary<string, Cell>();
        private DependencyGraph DG = new DependencyGraph();
        private Formula.Lookup SSDelegate;

        /// <summary>
        /// Delegate function for looking up cell values in the spreadsheet
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        private double SSDel(string name)
        {
            if (double.TryParse(GetCellValue(name).ToString(), out double result))
                return result;
            else throw new ArgumentException();

        }

        /// <summary>
        /// True if this spreadsheet has been modified since it was created or saved                  
        /// (whichever happened most recently); false otherwise.
        /// </summary>
        public override bool ChangedSinceSave { get; protected set; } = false;

        /// <summary>
        /// If name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, returns the value (as opposed to the contents) of the named cell.  The return
        /// value should be either a string, a double, or a SpreadsheetUtilities.FormulaError.
        /// </summary>
        public override object GetCellValue(string name)
        {
            if (name == null)
                throw new InvalidCellNameException();
            Cell empty = new Cell(name, ""); //cell performs the name validity check
            if (nonEmptyCells.TryGetValue(name, out Cell output))
            {
                if (output.Contents.GetType() == typeof(Formula))
                    return ((Formula)output.Contents).Evaluate(SSDelegate);
                else return output.CellValue;
            }
            return "";   //name must be valid to get here, but there is no nonempty cell, default contents are ""
        }

        /// <summary>
        /// If content is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, if content parses as a double, the contents of the named
        /// cell becomes that double.
        /// 
        /// Otherwise, if content begins with the character '=', an attempt is made
        /// to parse the remainder of content into a Formula f using the Formula
        /// constructor.  There are then three possibilities:
        /// 
        ///   (1) If the remainder of content cannot be parsed into a Formula, a 
        ///       SpreadsheetUtilities.FormulaFormatException is thrown.
        ///       
        ///   (2) Otherwise, if changing the contents of the named cell to be f
        ///       would cause a circular dependency, a CircularException is thrown.
        ///       
        ///   (3) Otherwise, the contents of the named cell becomes f.
        /// 
        /// Otherwise, the contents of the named cell becomes content.
        /// 
        /// If an exception is not thrown, the method returns a set consisting of
        /// name plus the names of all other cells whose value depends, directly
        /// or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        public override ISet<string> SetContentsOfCell(string name, string content)
        {
            if (content == null)
                throw new ArgumentNullException();
            try
            {
                ChangedSinceSave = true;
                name = NormalizeCellName(name);
                if (!IsValid(name))
                    throw new InvalidCellNameException();
                if (double.TryParse(content, out double result))
                    return SetCellContents(name, result);
                if (content.StartsWith("="))
                    return SetCellContents(name, new Formula(content.Substring(1), NormalizeCellName, IsValid));
                else return SetCellContents(name, content);
            }
            catch (InvalidCellNameException e) { throw e; }
        }

        /// <summary>
        /// If name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, returns the contents (as opposed to the value) of the named cell.  The return
        /// value should be either a string, a double, or a Formula.
        public override object GetCellContents(string name)
        {
            if (name == null)
                throw new InvalidCellNameException();
            if (nonEmptyCells.TryGetValue(NormalizeCellName(name), out Cell output))
                return output.Contents;
            else return new Cell(name, "").Contents;
            /*if the name is invalid the "Cell" class will throw an exception since every name is checked when instantiated,
             if valid, it won't be added to the dictionary since it is empty. This heuristic is implemented in several functions
             for throwing an error*/
        }

        /// <summary>
        /// If name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, the contents of the named cell becomes number.  The method returns a
        /// set consisting of name plus the names of all other cells whose value depends, 
        /// directly or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        protected override ISet<string> SetCellContents(string name, double number)
        {
            if (name != null)
            {
                if (nonEmptyCells.TryGetValue(name, out Cell output))
                    output.Contents = output.CellValue = number;
                else
                    nonEmptyCells[name] = new Cell(name, number) { CellValue = number };
                DG.ReplaceDependees(name, null);
                return new HashSet<string>(GetCellsToRecalculate(name));
            }
            throw new InvalidCellNameException();
        }

        /// <summary>
        /// If text is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, the contents of the named cell becomes text.  The method returns a
        /// set consisting of name plus the names of all other cells whose value depends, 
        /// directly or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        protected override ISet<string> SetCellContents(string name, string text)
        {
            if (text == null)
                throw new ArgumentNullException();
            if (text == "")
                return new HashSet<string>() { name };
            if (name != null)
            {
                if (nonEmptyCells.TryGetValue(name, out Cell output))
                    output.Contents = output.CellValue = text;
                else
                    nonEmptyCells[name] = new Cell(name, text) { CellValue = text };
                DG.ReplaceDependees(name, null);
                return new HashSet<string>(GetCellsToRecalculate(name));
            }
            throw new InvalidCellNameException();

        }

        /// <summary>
        /// If the formula parameter is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name is null or invalid, throws an InvalidNameException.
        /// 
        /// Otherwise, if changing the contents of the named cell to be the formula would cause a 
        /// circular dependency, throws a CircularException.  (No change is made to the spreadsheet.)
        /// 
        /// Otherwise, the contents of the named cell becomes formula.  The method returns a
        /// Set consisting of name plus the names of all other cells whose value depends,
        /// directly or indirectly, on the named cell.
        /// 
        /// For example, if name is A1, B1 contains A1*2, and C1 contains B1+A1, the
        /// set {A1, B1, C1} is returned.
        /// </summary>
        protected override ISet<string> SetCellContents(string name, Formula formula)
        {
            if (formula == null)
                throw new ArgumentNullException();
            if (name != null)
            {
                if (nonEmptyCells.TryGetValue(name, out Cell output))
                {
                    object tempSaveC = output.Contents;
                    HashSet<string> tempSaveD = new HashSet<string>(DG.GetDependees(name));
                    output.Contents = formula;
                    output.CellValue = formula.Evaluate(SSDelegate);
                    DG.ReplaceDependees(name, formula.GetVariables());
                    try
                    {
                        return new HashSet<string>(GetCellsToRecalculate(name));
                    }
                    catch (CircularException)
                    {
                        output.Contents = tempSaveC;
                        DG.ReplaceDependees(name, tempSaveD);
                        throw new CircularException();
                    }
                }
                else
                    nonEmptyCells[name] = new Cell(name, formula) { CellValue = formula.Evaluate(SSDelegate) };
                DG.ReplaceDependees(name, formula.GetVariables());
                return new HashSet<string>(GetCellsToRecalculate(name));
            }
            throw new InvalidCellNameException();
        }

        private void SetHelper(string name, object contents)
        {
            if (nonEmptyCells.TryGetValue(name, out Cell output))
                output.Contents = contents;
            else
                nonEmptyCells[name] = new Cell(name, contents);
        }

        /// <summary>
        /// Enumerates the names of all the non-empty cells in the spreadsheet.
        /// </summary>
        public override IEnumerable<string> GetNamesOfAllNonemptyCells()
        {
            foreach (var pair in nonEmptyCells)
                yield return pair.Key;
        }

        /// <summary>
        /// If name is null, throws an ArgumentNullException.
        /// 
        /// Otherwise, if name isn't a valid cell name, throws an InvalidNameException.
        /// 
        /// Otherwise, returns an enumeration, without duplicates, of the names of all cells whose
        /// values depend directly on the value of the named cell.  In other words, returns
        /// an enumeration, without duplicates, of the names of all cells that contain
        /// formulas containing name.
        /// 
        /// For example, suppose that
        /// A1 contains 3
        /// B1 contains the formula A1 * A1
        /// C1 contains the formula B1 + A1
        /// D1 contains the formula B1 - C1
        /// The direct dependents of A1 are B1 and C1
        /// </summary>
        protected override IEnumerable<string> GetDirectDependents(string name)
        {
            if (name == null)
                throw new ArgumentNullException();
            if (DG.HasDependents(name))
                foreach (string dent in DG.GetDependents(name))
                    yield return dent;
        }




        /// <summary>
        /// Returns the version information of the spreadsheet saved in the named file.
        /// If there are any problems opening, reading, or closing the file, the method
        /// should throw a SpreadsheetReadWriteException with an explanatory message.
        /// </summary>
        public override string GetSavedVersion(string filename)
        {
            //filename += ".xml";
            using (XmlReader reader = XmlReader.Create(filename))
            {
                try
                {
                    while (reader.Read())
                    {
                        if (reader.IsStartElement())
                        {
                            switch (reader.Name)
                            {
                                case "spreadsheet":
                                    reader.MoveToAttribute("version");
                                    return reader.ReadInnerXml();
                            }
                        }
                    }
                    throw new SpreadsheetReadWriteException("file has no version information");
                }
                catch (Exception e)
                {
                    throw new SpreadsheetReadWriteException(e.Message);
                }
            }
        }

        /// <summary>
        /// Writes the contents of this spreadsheet to the named file using an XML format.
        /// The XML elements should be structured as follows:
        /// 
        /// <spreadsheet version="version information goes here">
        /// 
        /// <cell>
        /// <name>
        /// cell name goes here
        /// </name>
        /// <contents>
        /// cell contents goes here
        /// </contents>    
        /// </cell>
        /// 
        /// </spreadsheet>
        /// 
        /// There should be one cell element for each non-empty cell in the spreadsheet.  
        /// If the cell contains a string, it should be written as the contents.  
        /// If the cell contains a double d, d.ToString() should be written as the contents.  
        /// If the cell contains a Formula f, f.ToString() with "=" prepended should be written as the contents.
        /// 
        /// If there are any problems opening, writing, or closing the file, the method should throw a
        /// SpreadsheetReadWriteException with an explanatory message.
        /// </summary>
        public override void Save(string filename)
        {
            try
            {
                XmlWriterSettings settings = new XmlWriterSettings { Indent = true, IndentChars = ("  ") };
                using (XmlWriter writer = XmlWriter.Create(filename /*+= ".xml"*/, settings))
                {

                    writer.WriteStartDocument();
                    writer.WriteStartElement("spreadsheet");
                    writer.WriteAttributeString("version", Version);
                    foreach (Cell cell in nonEmptyCells.Values)
                        cell.WriteXml(writer);
                    writer.WriteEndElement();

                }
            }
            catch (Exception e)
            {
                throw new SpreadsheetReadWriteException(e.Message);
            }
            ChangedSinceSave = false;
        }


        /// <summary>
        /// Helper Function that aids the constructor of which a filename argument is provided. Creates a spreadsheet based on the contents of the file.
        /// </summary>
        /// <param name="filename"></param>
        public void ConstructFromSaved(string filename)
        {
            //filename += ".xml";
            try
            {
                using (XmlReader reader = XmlReader.Create(filename))
                {

                    while (reader.Read())
                    {
                        if (reader.IsStartElement())
                        {

                            switch (reader.Name)
                            {
                                case "cell":
                                    reader.ReadToDescendant("name");
                                    string name = reader.ReadInnerXml();
                                    reader.MoveToContent();
                                    string contents = reader.ReadInnerXml();
                                    name = NormalizeCellName(name);
                                    SetContentsOfCell(name, contents);
                                    break;
                                case "spreadsheet":
                                    reader.MoveToAttribute("version");
                                    if (reader.ReadInnerXml() != Version)
                                        throw new SpreadsheetReadWriteException("Invalid Version");
                                    break;

                            }
                        }
                    }

                }
            }
            catch (Exception e)
            {
                throw new SpreadsheetReadWriteException(e.Message);
            }
            ChangedSinceSave = false;
        }







    }
}
