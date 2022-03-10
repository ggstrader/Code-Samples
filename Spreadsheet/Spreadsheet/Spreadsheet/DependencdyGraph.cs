using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace SpreadsheetUtilities
{
    public class DependencyGraph : IEnumerable
    {
        public DependencyGraph() { }
        /// <summary>
        /// contains one entry for each dependee, the hashset contains this dependee's dependants
        /// </summary>
        private Dictionary<string, HashSet<string>> MapDeetoDpendants = new Dictionary<string, HashSet<string>>();
        /// <summary>
        /// contains one entry for each dependant, the hashset contains this dependant's dependees
        /// </summary>
        private Dictionary<string, HashSet<string>> MapDenttoDependees = new Dictionary<string, HashSet<string>>();

        /// <summary>
        /// The number of ordered pairs in the DependencyGraph.
        /// </summary>
        public int Size
        {
            get
            {
                int total = 0;
                foreach (var dependee in MapDeetoDpendants)
                    total += dependee.Value.Count();
                return total;
            }
        }
        /// <summary>
        /// Returns the size of dependees("a")
        /// </summary>
        public int this[string s]
        {
            get
            {
                if (MapDenttoDependees.ContainsKey(s))
                    return MapDenttoDependees[s].Count();
                else return 0;
            }
        }

        /// <summary>
        /// Reports whether dependents(s) is non-empty.
        /// </summary>
        public bool HasDependents(string dependee)
        {
            return MapDeetoDpendants.ContainsKey(dependee);
        }
        /// <summary>
        /// Reports whether dependees(s) is non-empty.
        /// </summary>
        public bool HasDependees(string dependent)
        {
            return MapDenttoDependees.ContainsKey(dependent);
        }

        /// <summary>
        /// Enumerates dependents(s).
        /// </summary>
        public IEnumerable<string> GetDependents(string s)
        {
            if (MapDeetoDpendants.ContainsKey(s))
                foreach (string item in MapDeetoDpendants[s])
                    yield return item;
        }
        /// <summary>
        /// Enumerates dependees(s).
        /// </summary>
        public IEnumerable<string> GetDependees(string s)
        {
            if (MapDenttoDependees.ContainsKey(s))
                foreach (string item in MapDenttoDependees[s])
                    yield return item;
        }

        public void AddDependency(string dependee, string dependent)
        {
            bool hasDee = MapDeetoDpendants.ContainsKey(dependee);
            bool hasDent = MapDenttoDependees.ContainsKey(dependent);
            if (hasDee)
            {
                if (MapDeetoDpendants[dependee].Remove(dependent))
                    MapDenttoDependees[dependent].Remove(dependee);
                MapDeetoDpendants[dependee].Add(dependent);
            }
            else
                MapDeetoDpendants[dependee] = new HashSet<string> { dependent };
            if (hasDent)
                MapDenttoDependees[dependent].Add(dependee);
            else MapDenttoDependees[dependent] = new HashSet<string> { dependee };
        }
        public void RemoveDependency(string dependee, string dependent)
        {
            RemoveHelp(MapDeetoDpendants, dependee, dependent);
            RemoveHelp(MapDenttoDependees, dependent, dependee);

        }
        private void RemoveHelp(Dictionary<string, HashSet<string>> d, string first, string second)
        {
            if (d.ContainsKey(first))
                if (d[first].Contains(second))
                {
                    d[first].Remove(second);
                    if (d[first].Count == 0)
                        d.Remove(first);
                }
        }
        public void ReplaceDependents(string dependee, IEnumerable<string> newDependents)
        {
            ReplaceHelp(MapDeetoDpendants, MapDenttoDependees, dependee);
            if (newDependents != null)
                foreach (var dependent in newDependents)
                    AddDependency(dependee, dependent);
            else MapDeetoDpendants.Remove(dependee);
        }
        public void ReplaceDependees(string dependent, IEnumerable<string> newDependees)
        {
            ReplaceHelp(MapDenttoDependees, MapDeetoDpendants, dependent);
            if (newDependees != null)
                foreach (var dependee in newDependees)
                    AddDependency(dependee, dependent);
            else MapDenttoDependees.Remove(dependent);
        }
        private void ReplaceHelp(Dictionary<string, HashSet<string>> TargDict, Dictionary<string, HashSet<string>> SupDict, string target)
        {
            if (TargDict.ContainsKey(target))
                foreach (string child in TargDict[target])
                    SupDict[child].Remove(target);
            TargDict[target] = new HashSet<string>();
        }

        /// <summary>
        /// Enumerating yields the names of all Dependees contained in the graph.
        /// </summary>
        /// <returns></returns>
        public IEnumerator GetEnumerator()
        {
            return ((IEnumerable)MapDeetoDpendants).GetEnumerator();
        }
    }

}