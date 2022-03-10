using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace SpreadsheetUtilities
{
    /// <summary>
    /// Represents formulas written in standard infix notation using standard precedence
    /// rules.  The allowed symbols are non-negative numbers written using double-precision 
    /// floating-point syntax; variables that consist of a letter or underscore followed by 
    /// zero or more letters, underscores, or digits; parentheses; and the four operator 
    /// symbols +, -, *, and /.  
    /// 
    /// Spaces are significant only insofar that they delimit tokens.  For example, "xy" is
    /// a single variable, "x y" consists of two variables "x" and y; "x23" is a single variable; 
    /// and "x 23" consists of a variable "x" and a number "23".
    /// 
    /// Associated with every formula are two delegates:  a normalizer and a validator.  The
    /// normalizer is used to convert variables into a canonical form, and the validator is used
    /// to add extra restrictions on the validity of a variable (beyond the standard requirement 
    /// that it consist of a letter or underscore followed by zero or more letters, underscores,
    /// or digits.)  Their use is described in detail in the constructor and method comments.
    /// </summary>
    public class Formula
    {
        private IEnumerable<string> expression { get; set; }

        public Formula(String formula) : this(formula, s => s, s => true)
        { }

        public Formula(String formula, Func<string, string> normalize, Func<string, bool> isValid)
        {
            formula = normalize(formula);
            expression = GetTokens(formula);
            if (!IsValidFormula(expression))
                throw new FormulaFormatException("invalid formula syntax");
            foreach (var item in GetVariables())
                if (!isValid(item))
                    throw new FormulaFormatException("invalid formula syntax");

        }

        private bool IsValidFormula(IEnumerable<string> Tokens)
        {
            if (Tokens == null || Tokens.Count() < 1)
                return false;
            int rightParen = 0;
            int leftParen = 0;
            bool firstChar = true;
            string lastChar = "_";
            HashSet<string> vars = new HashSet<string>(GetVariables().ToList());
            foreach (string token in Tokens)
            {
                if (IsValidToken(token, vars))
                {
                    if (token == "(")
                        leftParen += 1;
                    if (token == ")")
                        rightParen += 1;
                    if (rightParen > leftParen)
                        return false;
                    if (lastChar == "(" || lastChar == "+" || lastChar == "-" || lastChar == "/" || lastChar == "*")
                        if (!double.TryParse(token, out double result0) && token != "(" && !vars.Contains(token))
                            return false;
                    if (double.TryParse(lastChar, out double result1) || lastChar == ")" || vars.Contains(lastChar))
                        if (token != ")" && token != "+" && token != "-" && token != "/" && token != "*")
                            return false;
                    if (firstChar)
                    {
                        if (token == "+" || token == "-" || token == "/" || token == "*")
                            return false;
                        firstChar = false;
                    }
                }
                else return false;
                lastChar = token;
            }
            if (leftParen != rightParen)
                return false;
            if (double.TryParse(lastChar, out double result) || lastChar == ")" || vars.Contains(lastChar))
                return true;
            return false;
        }

        private bool IsValidToken(string token, HashSet<string> vars)
        {
            if (double.TryParse(token, out double result) || token == "(" || token == ")" ||
                    token == "+" || token == "-" || token == "/" || token == "*" || token == "_"
                    || vars.Contains(token))
                return true;
            return false;
        }

        public delegate double Lookup(string name);

        /// <summary>
        /// Evaluates this Formula, using the lookup delegate to determine the values of
        /// variables. 
        /// 
        /// Given a variable symbol as its parameter, lookup returns the variable's value 
        /// (if it has one) or throws an ArgumentException (otherwise).
        /// 
        /// If no undefined variables or divisions by zero are encountered when evaluating 
        /// this Formula, the value is returned.  Otherwise, a FormulaError is returned.  
        /// </summary>
        public object Evaluate(Lookup lookup)
        {
            Stack<string> operators = new Stack<string>();
            Stack<string> operands = new Stack<string>();
            foreach (string item in expression)
            {
                if (item == " " || item == "")
                    continue;
                if (item == "+" || item == "-")
                {
                    if (operators.IsOnTop("+"))
                        PerformOp(operators, operands, (x, y) => { return x + y; });
                    if (operators.IsOnTop("-"))
                        PerformOp(operators, operands, (x, y) => { return x - y; });
                    operators.Push(item);
                    continue;
                }
                if (item == "*" || item == "/" || item == "(")
                { operators.Push(item); continue; }
                if (item == ")")
                {
                    if (operators.IsOnTop("+"))
                        PerformOp(operators, operands, (x, y) => { return x + y; });
                    if (operators.IsOnTop("-"))
                        PerformOp(operators, operands, (x, y) => { return x - y; });
                    if (operators.IsOnTop("("))
                        operators.Pop();
                    if (operators.IsOnTop("*"))
                        PerformOp(operators, operands, (x, y) => { return x * y; });
                    if (operators.IsOnTop("/"))
                    {
                        try { PerformOp(operators, operands, (x, y) => { return x / y; }); }
                        catch { return new FormulaError("Division by zero"); };
                    }
                    continue;
                }

                if (double.TryParse(item, out double result))
                {
                    operands.Push(item);
                    if (operators.IsOnTop("*"))
                        PerformOp(operators, operands, (x, y) => { return x * y; });
                    if (operators.IsOnTop("/"))
                    {
                        try { PerformOp(operators, operands, (x, y) => { return x / y; }); }
                        catch { return new FormulaError("Division by zero"); };
                    }

                    continue;
                }
                try
                {
                    string value = lookup(item).ToString();
                    operands.Push(value);
                    if (operators.IsOnTop("*"))
                        PerformOp(operators, operands, (x, y) => { return x * y; });
                    if (operators.IsOnTop("/"))
                    {
                        try { PerformOp(operators, operands, (x, y) => { return x / y; }); }
                        catch { return new FormulaError("Division by Zero"); };
                    }
                    continue;
                }
                catch (Exception)
                { return new FormulaError("Invalid variable name"); }
            }
            if (operators.Count == 0)
            {
                if (operands.Count == 1)
                    return double.Parse(operands.Pop());
                else return new FormulaError("Invalid Expression");
            }
            else
            {
                if (operands.Count == 2 && operators.Count == 1)
                {
                    if (operators.IsOnTop("+"))
                        PerformOp(operators, operands, (x, y) => { return x + y; });
                    if (operators.IsOnTop("-"))
                        PerformOp(operators, operands, (x, y) => { return x - y; });
                    return double.Parse(operands.Pop());
                }
                else return new FormulaError("Invalid Expression");
            }

        }

        private static void PerformOp<T>(Stack<T> operators, Stack<string> operands, Func<double, double, double> op)
        {
            operators.Pop();
            double second = double.Parse(operands.Pop());
            double first = double.Parse(operands.Pop());
            if (double.IsInfinity(op(first, second)))
                throw new DivideByZeroException();
            operands.Push(op(first, second).ToString());
        }

        /// <summary>
        /// Enumerates the normalized versions of all of the variables with no duplicates.
        /// </summary>
        public IEnumerable<String> GetVariables()
        {
            List<string> temp = new List<string>();

            foreach (var item in expression)
                if (!double.TryParse(item, out double result)
                    && item != "+" && item != "-"
                    && item != "*" && item != "/"
                    && item != "(" && item != ")")
                    if (!temp.Contains(item))
                    {
                        temp.Add(item);
                        yield return item;
                    }
        }

        /// <summary>
        /// Returns a string containing no spaces which, if passed to the Formula
        /// constructor, will produce a Formula f such that this.Equals(f).
        /// </summary>
        public override string ToString()
        {
            string result = "";
            foreach (var item in expression)
                if (double.TryParse(item, out double parsed))
                    result += parsed.ToString();
                else result += item;
            return result;
        }

        /// <summary>
        /// Two Formulae are considered equal if they consist of the same tokens in the
        /// same order.  
        /// Numeric tokens are considered equal by C#'s standard conversion from string to double.
        /// Variable tokens are considered equal if their normalized forms are equal, as 
        /// defined by the provided normalizer.
        /// </summary>
        public override bool Equals(object obj)
        {
            if (obj == null || !obj.GetType().Equals(typeof(Formula)))
                return false;
            var expression2 = GetTokens(obj.ToString());
            int count = expression2.Count();
            if (count != expression.Count())
                return false;
            for (int i = 0; i < count; i++)
            {
                string first = expression.ElementAt(i);
                string second = expression2.ElementAt(i);
                if (double.TryParse(first, out double result))
                {
                    if (double.TryParse(second, out double result2))
                    {
                        if (result.ToString() == result2.ToString())
                            continue;
                    }
                    return false;
                }
                if (!first.Equals(second))
                    return false;
            }
            return true;
        }

        public static bool operator ==(Formula f1, Formula f2)
        {
            if (ReferenceEquals(f1, null))
                return ReferenceEquals(f2, null);
            else return f1.Equals(f2);
        }

        public static bool operator !=(Formula f1, Formula f2)
        {
            return !(f1 == f2);
        }

        /// <summary>
        /// Returns a hash code for this Formula.  If f1.Equals(f2), then it must be the
        /// case that f1.GetHashCode() == f2.GetHashCode(). 
        /// </summary>
        public override int GetHashCode()
        {
            return this.ToString().GetHashCode();
        }
    
        public static IEnumerable<string> GetTokens(String formula)
        {
            String lpPattern = @"\(";
            String rpPattern = @"\)";
            String opPattern = @"[\+\-*/]";
            String varPattern = @"[a-zA-Z_](?: [a-zA-Z_]|\d)*";
            String doublePattern = @"(?: \d+\.\d* | \d*\.\d+ | \d+ ) (?: [eE][\+-]?\d+)?";
            String spacePattern = @"\s+";

            String pattern = String.Format("({0}) | ({1}) | ({2}) | ({3}) | ({4}) | ({5})",
                                            lpPattern, rpPattern, opPattern, varPattern, doublePattern, spacePattern);

            foreach (String s in Regex.Split(formula, pattern, RegexOptions.IgnorePatternWhitespace))
                if (!Regex.IsMatch(s, @"^\s*$", RegexOptions.Singleline))
                    yield return s;
        }
    }

    public class FormulaFormatException : Exception
    {
        public FormulaFormatException(String message) : base(message) { }
    }

    public struct FormulaError
    {
        public FormulaError(String reason) : this()
        {
            Reason = reason;
            Console.WriteLine(reason);
        }
        public string Reason { get; private set; }
    }

}



static class Extensions
{
    public static bool IsOnTop<T>(this Stack<T> stck, T val)
    {
        if (stck.Count > 0 && stck.Peek().Equals(val))
            return true;
        else return false;
    }
}


