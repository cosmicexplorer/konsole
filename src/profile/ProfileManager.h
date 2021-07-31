/*
    This source file is part of Konsole, a terminal emulator.

    SPDX-FileCopyrightText: 2006-2008 Robert Knight <robertknight@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

// Qt
#include <QKeySequence>
#include <QHash>
#include <QList>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <QStack>

#include <set>

// Konsole
#include "Profile.h"

namespace Konsole {
/**
 * Manages profiles which specify various settings for terminal sessions
 * and their displays.
 */
class KONSOLEPROFILE_EXPORT ProfileManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructs a new profile manager and loads information about the available
     * profiles.
     */
    ProfileManager();

    /**
     * Destroys the ProfileManager.
     */
    ~ProfileManager() override;

    /**
     * Returns the profile manager instance.
     */
    static ProfileManager *instance();

    /**
     * Returns a list of all available profiles
     *
     * Initially only the profile currently set as the default is loaded.
    *
     * When this method is called, it calls loadAllProfiles() internally to
     * ensure all available profiles are loaded and usable.
     */
    QList<Profile::Ptr> allProfiles();

    /**
     * Returns a list of already loaded profiles
     */
    QList<Profile::Ptr> loadedProfiles() const;

    /**
     * Loads all available profiles.  This involves reading each
     * profile configuration file from disk and parsing it.
     * Therefore it should only be done when necessary.
     */
    void loadAllProfiles();

    /**
     * Loads a profile from the specified path and registers
     * it with the ProfileManager.
     *
     * @p path may be relative or absolute.  The path may just be the
     * base name of the profile to load (eg. if the profile's full path
     * is "<konsole data dir>/My Profile.profile" then both
     * "konsole/My Profile.profile" , "My Profile.profile" and
     * "My Profile" will be accepted)
     *
     * @return Pointer to a profile which can be passed to
     * SessionManager::createSession() to create a new session using
     * this profile.
     */
    Profile::Ptr loadProfile(const QString &shortPath);

    /**
     * This creates a profile based on the fallback profile, it's shown
     * as "Default". This is a special profile as it's not saved on disk
     * but rather created from code in the Profile class, based on the default
     * profile settings.
     */
    void initFallbackProfile();

    /**
     * Searches for available profiles on-disk and returns a list
     * of paths of profiles which can be loaded.
     */
    QStringList availableProfilePaths() const;

    /**
     * Returns a list of names of all available profiles
     */
    QStringList availableProfileNames() const;

    /**
     * Registers a new type of session.
     */
    void addProfile(const Profile::Ptr &profile);

    /**
     * Deletes the configuration file used to store a profile.
     * The profile will continue to exist while sessions are still using it.  The profile
     * will be marked as hidden (see Profile::setHidden() ) so that it does not show
     * up in profile lists and future changes to the profile are not stored to disk.
     *
     * Returns true if the profile was successfully deleted or false otherwise.
     */
    bool deleteProfile(Profile::Ptr profile);

    /**
     * Updates a @p profile with the changes specified in @p propertyMap.
     *
     * All sessions currently using the profile will be updated to reflect the new settings.
     *
     * After the profile is updated, the profileChanged() signal will be emitted.
     *
     * @param profile The profile to change
     * @param propertyMap A map between profile properties and values describing the changes
     * @param persistent If true, the changes are saved to the profile's configuration file,
     * set this to false if you want to preview possible changes to a profile but do not
     * wish to make them permanent.
     */
    void changeProfile(Profile::Ptr profile, QHash<Profile::Property, QVariant> propertyMap,
                       bool persistent = true);

    /**
     * Sets the @p profile as the default profile for creating new sessions
     */
    void setDefaultProfile(const Profile::Ptr &profile);

    /**
     * Returns a Profile object describing the default profile
     */
    Profile::Ptr defaultProfile() const;

    /**
     * Returns a Profile object with hard-coded settings which is always available.
     * This can be used as a parent for new profiles which provides suitable default settings
     * for all properties.
     */
    Profile::Ptr fallbackProfile() const;

    /**
     * Associates a shortcut with a particular profile.
     */
    void setShortcut(Profile::Ptr profile, const QKeySequence &keySequence);

    /** Returns the shortcut associated with a particular profile. */
    QKeySequence shortcut(Profile::Ptr profile) const;

    /**
     * Creates a unique name for a new profile, e.g. "Profile 1", "Profile 2" ...etc.
     */
    QString generateUniqueName() const;

Q_SIGNALS:

    /** Emitted when a profile is added to the manager. */
    void profileAdded(const Profile::Ptr &ptr);
    /** Emitted when a profile is removed from the manager. */
    void profileRemoved(const Profile::Ptr &ptr);
    /** Emitted when a profile's properties are modified. */
    void profileChanged(const Profile::Ptr &ptr);

    /**
     * Emitted when the shortcut for a profile is changed.
     *
     * @param profile The profile whose status was changed
     * @param newShortcut The new shortcut key sequence for the profile
     */
    void shortcutChanged(const Profile::Ptr &profile, const QKeySequence &newShortcut);

public Q_SLOTS:
    /** Saves settings (shortcuts, default profile etc.) to disk. */
    void saveSettings();

protected Q_SLOTS:

private Q_SLOTS:

private:
    Q_DISABLE_COPY(ProfileManager)

    // loads the mappings between shortcut key sequences and
    // profile paths
    void loadShortcuts();
    // saves the mappings between shortcut key sequences and
    // profile paths
    void saveShortcuts();

    // records which profile is set as the default profile
    // Note: it does not save the profile itself into disk. That is
    // what saveProfile() does.
    void saveDefaultProfile();

    // saves a profile to a file
    // returns the path to which the profile was saved, which will
    // be the same as the path property of profile if valid or a newly generated path
    // otherwise
    QString saveProfile(const Profile::Ptr &profile);

    static bool profileNamesCompare(const Profile::Ptr &p1, const Profile::Ptr &p2)
    {
        // Always put the Default/fallback profile at the top
        if (p1->isFallback()) {
            return true;
        } else if (p2->isFallback()) {
            return false;
        }

        return QString::localeAwareCompare(p1->name(), p2->name()) < 0;
    }

    // A list of all loaded profiles, sorted by profile name
    std::set<Profile::Ptr, decltype(profileNamesCompare) *> _profiles{profileNamesCompare};

    // This is called after renaming a profile, and saving it to the new
    // .profile on disk; this method will then delete the old profile (i.e.
    // remove the old .profile from disk ...etc), and assign the keyboard
    // shortcut (if any) of the old profile to the newly created one.
    void processProfileRenaming(Profile::Ptr profile, const QString &origPath);

    Profile::Ptr _defaultProfile;
    Profile::Ptr _fallbackProfile;

    bool _loadedAllProfiles; // set to true after loadAllProfiles has been called

    struct ShortcutData {
        Profile::Ptr profileKey;
        QString profilePath;
    };
    QMap<QKeySequence, ShortcutData> _shortcuts; // shortcut keys -> profile path

    // finds out if it's a internal profile or an external one,
    // fixing the path to point to the correct location for the profile.
    QString normalizePath(const QString& path) const;
};

}

#endif //PROFILEMANAGER_H
