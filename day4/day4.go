package main

import (
    "fmt"
    "os"
    "bufio"
    "regexp"
    "strconv"
    "sort"
    "github.com/willf/bitset"
    "time"
)

const (
    ACTION_BEGIN_SHIFT string = "BEGIN_SHIFT"
    ACTION_FALLS_ASLEEP string = "FALL_ASLEEP"
    ACTION_WAKES_UP string = "WAKES_UP"

    STATE_AWAKE string = "AWAKE"
    STATE_SLEEPING string = "SLEEPING"
)

var ACTION_REGEX *regexp.Regexp = regexp.MustCompile("\\[(\\d+)-(\\d+)-(\\d+) (\\d+):(\\d+)\\] (.*)")
// var ACTION_REGEX *regexp.Regexp = regexp.MustCompile("[(\\d+)(\\w+)")
var ACTION_SHIFT_REGEX *regexp.Regexp = regexp.MustCompile("Guard #(\\d+) begins shift")

type Action struct {
    time time.Time

    action string
    id uint
}

type Shift struct {
    guard uint

    start time.Time

    sleepSchedule *bitset.BitSet
}

func parseLine(line string) (Action, error) {
    action := Action{}

    matches := ACTION_REGEX.FindStringSubmatch(line)

    year, err := strconv.ParseUint(matches[1], 10, 32)
    if err != nil {
        return action, err
    }
    month, err := strconv.ParseInt(matches[2], 10, 32)
    if err != nil {
        return action, err
    }
    day, err := strconv.ParseInt(matches[3], 10, 32)
    if err != nil {
        return action, err
    }
    hour, err := strconv.ParseInt(matches[4], 10, 32)
    if err != nil {
        return action, err
    }
    minute, err := strconv.ParseInt(matches[5], 10, 32)
    if err != nil {
        return action, err
    }

    action.time = time.Date(int(year), time.Month(month), int(day), int(hour), int(minute), 0, 0, time.UTC)

    actionText := matches[6]

    if (actionText == "wakes up") {
        action.action = ACTION_WAKES_UP
    } else if (actionText == "falls asleep") {
        action.action = ACTION_FALLS_ASLEEP
    } else {
        id_matches := ACTION_SHIFT_REGEX.FindStringSubmatch(actionText)
        id, err := strconv.ParseUint(id_matches[1], 10, 32)
        if err != nil {
            return action, err
        }
        action.id = uint(id)
        action.action = ACTION_BEGIN_SHIFT
    }

    return action, nil
}

func updateShift(shift *Shift, state string, start_time, end_time time.Time) {
    for i := start_time.Minute(); i < end_time.Minute(); i++ {
        if state == STATE_AWAKE {
            shift.sleepSchedule = shift.sleepSchedule.Clear(uint(i))
        } else {
            shift.sleepSchedule = shift.sleepSchedule.Set(uint(i))
        }
    }
}

func buildShifts(actions []Action) []Shift {
    var current_shift *Shift = nil
    shifts := []Shift{}
    last_time := time.Time{}
    last_state := STATE_AWAKE

    for _, action := range actions {
        if action.action == ACTION_BEGIN_SHIFT {
            if current_shift != nil {
                end_time := current_shift.start.Add(time.Duration(1) * time.Hour)
                updateShift(current_shift, last_state, last_time, end_time)
                shifts = append(shifts, *current_shift)
            }

            current_shift = &Shift{}
            current_shift.sleepSchedule = bitset.New(60)
            last_state = STATE_AWAKE
            last_time = action.time
            current_shift.guard = action.id
            current_shift.start = action.time

            if current_shift.start.Hour() != 0 {
                current_shift.start.Add(time.Duration(1) * time.Hour * 24)
            }
            year, month, day := current_shift.start.Date()
            current_shift.start = time.Date(year, month, day, 0, 0, 0, 0, time.UTC)

        } else {
            updateShift(current_shift, last_state, last_time, action.time)
            last_time = action.time

            if action.action == ACTION_WAKES_UP {
                last_state = STATE_AWAKE
            }
            if action.action == ACTION_FALLS_ASLEEP {
                last_state = STATE_SLEEPING
            }
        }
    }

    return shifts
}

func part1(shifts []Shift) uint {
    asleepStat := map[uint]uint{}
    scheduleStat := map[uint][]*bitset.BitSet{}
    biggestSleeper := uint(0)
    biggestSleeperMinutes := uint(0)

    for _, shift := range shifts {
        asleepStat[shift.guard] += shift.sleepSchedule.Count()
        scheduleStat[shift.guard] = append(scheduleStat[shift.guard], shift.sleepSchedule)

        if asleepStat[shift.guard] > biggestSleeperMinutes {
            biggestSleeper = shift.guard
            biggestSleeperMinutes = asleepStat[shift.guard]
        }
    }

    biggestSleepingMinute := uint(0)
    biggestSleepingMinuteCount := uint(0)
    sleepingMinutes := map[uint]uint{}

    for _, schedule := range scheduleStat[biggestSleeper] {
        for i := uint(0); i < 60; i++ {
            if schedule.Test(i) {
                sleepingMinutes[i]++

                if sleepingMinutes[i] > biggestSleepingMinuteCount {
                    biggestSleepingMinute = i
                    biggestSleepingMinuteCount = sleepingMinutes[i]
                }
            }
        }
    }

    return biggestSleepingMinute * biggestSleeper
}

func main() {
    if len(os.Args) != 2 {
        fmt.Println("Please provide the file in arguments...")
        os.Exit(1)
    }

    filename := os.Args[1]

    file, err := os.Open(filename)
    if err != nil {
        fmt.Println("{}", err)
        os.Exit(1)
    }
    defer file.Close()

    scanner := bufio.NewScanner(file)

    actions := []Action{}
    for scanner.Scan() {
        action, err := parseLine(scanner.Text())
        if err != nil {
            fmt.Println("{}", err)
            os.Exit(1)
        }
        actions = append(actions, action)
    }

    sort.Slice(actions, func(i, j int) bool {
        return actions[i].time.Before(actions[j].time)
    })

    shifts := buildShifts(actions)

    fmt.Printf("Part 1: %d\n", part1(shifts))
}
