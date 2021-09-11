
# Currently we are in the Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: ly_get_list_relative_pal_filename will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here in our gem : Gems/DazToO3DE/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/Gems/DazToO3DE/Code
ly_get_list_relative_pal_filename(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} ${o3de_gem_restricted_path} ${o3de_gem_path} ${o3de_gem_name})

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Add the DazToO3DE.Static target
# Note: We include the common files and the platform specific files which are set in daztoo3de_common_files.cmake
# and in ${pal_dir}/daztoo3de_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
ly_add_target(
    NAME DazToO3DE.Static STATIC
    NAMESPACE Gem
    FILES_CMAKE
        daztoo3de_files.cmake
        ${pal_dir}/daztoo3de_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            AZ::AzCore
            AZ::AzFramework
)

# Here add DazToO3DE target, it depends on the DazToO3DE.Static
ly_add_target(
    NAME DazToO3DE ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        daztoo3de_shared_files.cmake
        ${pal_dir}/daztoo3de_shared_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PRIVATE
            Gem::DazToO3DE.Static
)

# By default, we will specify that the above target DazToO3DE would be used by
# Client and Server type targets when this gem is enabled.  If you don't want it
# active in Clients or Servers by default, delete one of both of the following lines:
ly_create_alias(NAME DazToO3DE.Clients NAMESPACE Gem TARGETS Gem::DazToO3DE)
ly_create_alias(NAME DazToO3DE.Servers NAMESPACE Gem TARGETS Gem::DazToO3DE)

# If we are on a host platform, we want to add the host tools targets like the DazToO3DE.Editor target which
# will also depend on DazToO3DE.Static
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    ly_add_target(
        NAME DazToO3DE.Editor.Static STATIC
        NAMESPACE Gem
        FILES_CMAKE
            daztoo3de_editor_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                AZ::AzToolsFramework
                Gem::DazToO3DE.Static
    )

    ly_add_target(
        NAME DazToO3DE.Editor GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            daztoo3de_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::DazToO3DE.Editor.Static
    )

    # By default, we will specify that the above target DazToO3DE would be used by
    # Tool and Builder type targets when this gem is enabled.  If you don't want it
    # active in Tools or Builders by default, delete one of both of the following lines:
    ly_create_alias(NAME DazToO3DE.Tools    NAMESPACE Gem TARGETS Gem::DazToO3DE.Editor)
    ly_create_alias(NAME DazToO3DE.Builders NAMESPACE Gem TARGETS Gem::DazToO3DE.Editor)


endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for DazToO3DE.Static
    if(PAL_TRAIT_DAZTOO3DE_TEST_SUPPORTED)
        # We support DazToO3DE.Tests on this platform, add DazToO3DE.Tests target which depends on DazToO3DE.Static
        ly_add_target(
            NAME DazToO3DE.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                daztoo3de_files.cmake
                daztoo3de_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    Source
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    Gem::DazToO3DE.Static
        )

        # Add DazToO3DE.Tests to googletest
        ly_add_googletest(
            NAME Gem::DazToO3DE.Tests
        )
    endif()

    # If we are a host platform we want to add tools test like editor tests here
    if(PAL_TRAIT_BUILD_HOST_TOOLS)
        # We are a host platform, see if Editor tests are supported on this platform
        if(PAL_TRAIT_DAZTOO3DE_EDITOR_TEST_SUPPORTED)
            # We support DazToO3DE.Editor.Tests on this platform, add DazToO3DE.Editor.Tests target which depends on DazToO3DE.Editor
            ly_add_target(
                NAME DazToO3DE.Editor.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    daztoo3de_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        Source
                BUILD_DEPENDENCIES
                    PRIVATE
                        AZ::AzTest
                        Gem::DazToO3DE.Editor
            )

            # Add DazToO3DE.Editor.Tests to googletest
            ly_add_googletest(
                NAME Gem::DazToO3DE.Editor.Tests
            )
        endif()
    endif()
endif()
