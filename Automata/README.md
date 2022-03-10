## Automata (Wip)

This project is a work-in-progress plugin for the Obsidian note-taking appilcation. It currently allows task creation using "natural language processing" in many ways that Todoist (my previous task management tool) just doesn't allow.

When writing notes, and something comes to mind, you can simply hit 'enter' and begin typing the task, and when you're ready to assign a date (or use no date at all) type '@' followed by a date/recurrance/nothing and hit enter to create a task.

Automata supports much that Todoist can't handle, like exclusions and holidays. A few examples of crazier dates and recurrances Automata can handle:
- "do something @Easter 2028"
- "do something @every month on 2nd,3rd sat ending 2022"
- "do something @every nov,feb on the last fri"
- "do something @every 5,3 wed,fri starting yesterday until next october excluding next wed,fri"
- "do something @Every mothers day until 2050AD beginning about 20 years back except every other mothers day starting next year until 2030AD"
- "do something @Every Halloween Ending july 4th 2054 Starting around 5 [[halloweens]] back (at about 5pm) except every other Halloween" (Obsidian uses [[to link to other notes]] so that just demonstrates that automata ignores links)


Automata allows for rules as well. Currently the only rule implemented allows for mapping directories. i.e. If I'm in my sports directory and I create a task I can have it automatically added to a sports task board.

When typeing a recurrance/date Automata also shows a popup calendar to validate what days it will happen on (which is a feature Todoist also sorely lacks).