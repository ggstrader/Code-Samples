using System;
using System.Collections.Generic;
using SpreadsheetUtilities;

namespace SpreadsheetTemplate
{
    public class CircularException : Exception { }


    public class InvalidCellNameException : Exception
    {
    }
    public class SpreadsheetReadWriteException : Exception
    {
        public SpreadsheetReadWriteException(string msg)
            : base(msg)
        {
        }
    }
    public abstract class AbstractSpreadsheet
    {
        public abstract bool ChangedSinceSave { get; protected set; }
        public Func<string, bool> IsValid { get; protected set; }
        public Func<string, string> NormalizeCellName { get; protected set; }
        public string Version { get; protected set; }

        public AbstractSpreadsheet(Func<string, bool> isValid, Func<string, string> normalize, string version)
        {
            this.IsValid = isValid;
            this.NormalizeCellName = normalize;
            this.Version = version;
        }

        public abstract string GetSavedVersion(String filename);
        public abstract void Save(String filename);
        /// <summary>
        /// Return value should be either a string, a double, or a SpreadsheetUtilities.FormulaError.
        /// </summary>
        public abstract object GetCellValue(String name);
        /// <summary>
        /// The return value should be either a string, a double, or a Formula.
        /// </summary>
        public abstract object GetCellContents(String name);
        public abstract ISet<String> SetContentsOfCell(String name, String content);
        protected abstract ISet<String> SetCellContents(String name, double number);
        protected abstract ISet<String> SetCellContents(String name, String text);
        protected abstract ISet<String> SetCellContents(String name, Formula formula);

        public abstract IEnumerable<String> GetNamesOfAllNonemptyCells();
        /// <summary>
        /// Returns an enumeration, without duplicates, of the names of all cells whose
        /// values depend directly on the value of the named cell.
        /// </summary>
        protected abstract IEnumerable<String> GetDirectDependents(String name);
        protected IEnumerable<String> GetCellsToRecalculate(String name)
        {
            return GetCellsToRecalculate(new HashSet<String>() { name });
        }
        /// <summary>
        /// If any of the named cells are involved in a circular dependency,
        /// throws a CircularException.
        /// 
        /// Otherwise, returns an enumeration of the names of all cells whose values must
        /// be recalculated, assuming that the contents of each cell named in names has changed.
        /// The names are enumerated in the order in which the calculations should be done.  
        /// </summary>
        protected IEnumerable<String> GetCellsToRecalculate(ISet<String> names)
        {
            LinkedList<String> changed = new LinkedList<String>();
            HashSet<String> visited = new HashSet<String>();
            foreach (String name in names)
                if (!visited.Contains(name))
                    Visit(name, name, visited, changed);
            return changed;
        }
        private void Visit(String start, String name, ISet<String> visited, LinkedList<String> changed)
        {
            visited.Add(name);
            foreach (String n in GetDirectDependents(name))
                if (n.Equals(start))
                    throw new CircularException();
                else if (!visited.Contains(n))
                    Visit(start, n, visited, changed);
            changed.AddFirst(name);
        }

    }
}