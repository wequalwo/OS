#include <iostream>
#include <stdio.h>
#include <sys/errno.h>
#include <sys/acl.h>
#include <acl/libacl.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

#define PATHNAME "t"
#define EMPTY -314

int ID = 10000;
uid_t get_id_obj(std::string str)
{
    struct stat buf;
    if (stat(PATHNAME, &buf) == -1)
    {
        perror("stat");
        return -1;
    }
    std::cout << "  " + str << buf.st_uid << "\n";
    ID = buf.st_uid;
    return buf.st_uid;
}

int get_gr_name(gid_t id)
{
    struct group *gr_name = getgrgid(id);
    if (gr_name == NULL)
    {
        perror("getgrgid");
        return -1;
    }
    std::cout << "  group name: ";
    for (int i = 0; i < sizeof(gr_name->gr_name) / sizeof(gr_name->gr_name[0]); i++)
    {
        std::cout << gr_name->gr_name[i];
    }
    std::cout << "\n"
              << std::flush;
    return 0;
}

int get_us_name(gid_t id)
{
    struct passwd *us_name = getpwuid(id);
    if (us_name == NULL)
    {
        perror("getpwuid");
        return -1;
    }
    std::cout << "  user name: ";
    std::cout << "\n\n\n" << sizeof(us_name->pw_name) / size_t(us_name->pw_name[0]) << "\n\n\n";
    for (int i = 0; i < sizeof(us_name->pw_name) / size_t(us_name->pw_name[0]); i++)
    {
        std::cout << us_name->pw_name[i];
    }
    std::cout << "\n"
              << std::flush;
    return 0;
}

int analysis(acl_entry_t entry_p)
{
    // int id = 0;
    acl_tag_t tag_type_p;
    int res = acl_get_tag_type(entry_p, &tag_type_p);
    if (res == -1)
    {
        perror("acl_get_tag_type");
        return -1;
    }
    std::cout << "  Type: ";
    switch (tag_type_p)
    {
    case ACL_USER_OBJ:
    {
        std::cout << "ACL_USER_OBJ\n";
        unsigned int id = 0;
        id = get_id_obj("user_obj id: ");
        if (id == -1)
        {
            std::cout << "stat problems\n";
            return -1;
        }
        get_us_name((uid_t)id);
        break;
    }
    case ACL_USER:
    {
        std::cout << "ACL_USER\n";
        unsigned int *id = (unsigned int *)acl_get_qualifier(entry_p);
        if (id == NULL)
        {
            perror("acl_get_qualifier");
            return -1;
        }
        std::cout << "  user id: " << *id << "\n";
        get_us_name((uid_t)(*id));
        acl_free((void *)id);
        // delete name;
        break;
    }
    case ACL_GROUP_OBJ:
    {
        std::cout << "ACL_GROUP_OBJ\n";
        int id = get_id_obj("group_obj id: ");
        if (id == -1)
        {
            std::cout << "stat problems\n";
            return -1;
        }
        get_gr_name(id);
        break;
    }
    case ACL_GROUP:
    {
        std::cout << "ACL_GROUP\n";
        unsigned int *id = (unsigned int *)acl_get_qualifier(entry_p);
        if (id == NULL)
        {
            perror("acl_get_qualifier");
            return 1;
        }
        std::cout << "group id: " << *id << "; ";

        get_gr_name((gid_t)(*id));

        acl_free((void *)id);
        break;
    }
    case ACL_MASK:
        std::cout << "ACL_MASK\n";
        break;
    case ACL_OTHER:
        std::cout << "ACL_OTHER\n";
        break;
    }

    acl_permset_t permset_p;
    res = acl_get_permset(entry_p, &permset_p);
    if (res == -1)
    {
        perror("acl_get_permset");
        return -1;
    }
    std::cout << "  Access type: ";
    res = acl_get_perm(permset_p, ACL_READ);
    if (res == -1)
    {
        perror("acl_get_perm");
        return -1;
    }
    else if (res == 1)
    {
        std::cout << "ACL_READ ";
    }

    res = acl_get_perm(permset_p, ACL_WRITE);
    if (res == -1)
    {
        perror("acl_get_perm");
        return -1;
    }
    else if (res == 1)
    {
        std::cout << "ACL_WRITE ";
    }

    res = acl_get_perm(permset_p, ACL_EXECUTE);
    if (res == -1)
    {
        perror("acl_get_perm");
        return -1;
    }
    else if (res == 1)
    {
        std::cout << "ACL_EXECUTE";
    }
    std::cout << "\n";
    return 0;
}

void out(acl_t list)
{
    int res = 0;
    int count = 0;
    acl_entry_t entry_p;
    res = acl_get_entry(list, ACL_FIRST_ENTRY, &entry_p);
    if (res == -1)
    {
        perror("acl_get_entry");
        exit(EXIT_FAILURE);
    }
    std::cout << count << "\n";
    analysis(entry_p);
    count++;

    while (true)
    {
        res = acl_get_entry(list, ACL_NEXT_ENTRY, &entry_p);
        if (res == -1)
        {
            perror("acl_get_entry");
            exit(EXIT_FAILURE);
        }
        if (res == 0)
        {
            break;
        }
        std::cout << "\n"
                  << count << "\n";
        analysis(entry_p);
        count++;
    }
}

int y = 0;

int red(acl_t &list, acl_tag_t TAG, acl_perm_t PERM, acl_perm_t PERM2 = EMPTY, int _ID = ID)
{
    int res = 0;
    acl_entry_t entry_p;
    res = acl_create_entry(&list, &entry_p);
    if (res == -1)
    {
        perror("acl_create_entry");
        exit(EXIT_FAILURE);
    }

    res = acl_set_tag_type(entry_p, TAG);
    if (res == -1)
    {
        perror("acl_set_tag_type");
        exit(EXIT_FAILURE);
    }
    if (TAG == ACL_USER || TAG == ACL_GROUP)
    {
        acl_set_qualifier(entry_p, (const void *)(&_ID));
    }
    acl_permset_t permset_p;
    res = acl_get_permset(entry_p, &permset_p);
    if (res == -1)
    {
        perror("acl_get_permset");
        return -1;
    }
    res = acl_clear_perms(permset_p);
    if (res == -1)
    {
        perror("acl_clear_perms");
        exit(EXIT_FAILURE);
    }
    res = acl_add_perm(permset_p, PERM);
    if (res == -1)
    {
        perror("acl_add_perm");
        exit(EXIT_FAILURE);
    }
    if (PERM2 != EMPTY)
    {
        res = acl_add_perm(permset_p, PERM2);
        if (res == -1)
        {
            perror("acl_add_perm");
            exit(EXIT_FAILURE);
        }
    }
    res = acl_set_permset(entry_p, permset_p);
    if (res == -1)
    {
        perror("acl_set_permset");
        exit(EXIT_FAILURE);
    }

    res = acl_calc_mask(&list);
    if (res == -1)
    {
        perror("acl_calc_mask");
        exit(EXIT_FAILURE);
    }
    return 1;
}

void del(acl_t &list)
{
    int res = 0;
    acl_entry_t entry_del;
    while (true)
    {
        res = acl_get_entry(list, ACL_NEXT_ENTRY, &entry_del);
        if (res == -1)
        {
            perror("acl_get_entry");
            exit(EXIT_FAILURE);
        }
        if (res == 0)
        {
            break;
        }
        acl_delete_entry(list, entry_del);
    }
}

int main()
{
    int res = 0;
    std::cout << "Lab 9\n";
    acl_t list = acl_get_file(PATHNAME, ACL_TYPE_ACCESS);
    if (list == NULL)
    {
        perror("acl_get_file");
        exit(EXIT_FAILURE);
    }

    out(list);

    getchar();

    del(list); // let's delete all

    // let's create a new one
    red(list, ACL_MASK, ACL_READ);
    red(list, ACL_USER_OBJ, ACL_READ, ACL_WRITE);
    red(list, ACL_GROUP_OBJ, ACL_READ);
    red(list, ACL_USER, ACL_READ);
    red(list, ACL_OTHER, ACL_READ);

    res = acl_valid(list);
    if (res == -1)
    {
        perror("acl_valid");
        exit(EXIT_FAILURE);
    }
    res = acl_set_file(PATHNAME, ACL_TYPE_ACCESS, list);
    if (res == -1)
    {
        perror("acl_set_file");
        exit(EXIT_FAILURE);
    }

    std::cout << "\n\x1b[31mNew info:\n\x1b[0m";
    out(list);

    acl_free((void *)&list);
    return 0;
}